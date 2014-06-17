#!/usr/bin/env python2.7
import argparse
import parser
#import runtime
import proxy

def main():
    argparser = argparse.ArgumentParser(description="Compile file to bytecode")
    argparser.add_argument('filename', type=str, help="File to compile")
    args = argparser.parse_args()

    const = {}
    function = compile_function(const, [], parser.parse_file(args.filename))

    gktable = function.nonlocals.values()
    gktable.extend(const.values())
    gk = list()
    #gk = (runtime.Value*len(gktable))()
    for i, item in enumerate(gktable):
        if isinstance(item, Value):
            if isinstance(item.value, (unicode, str)):
                s = item.value.encode('utf-8')
                gk.append( proxy.box_cstring(s) )
            elif isinstance(item.value, (int, long, float)):
                gk.append( proxy.box_double(item.value) )
            else:
                raise Exception("um? %r" % item.value)
        else:
            gk.append( proxy.get_global(item.name) )

    #print gktable
    descriptor = compile_closure(function, gktable, gk, [])
    proxy.run_toplevel(descriptor)
    #cl = runtime.newClosure(cld, None)
    #runtime.callClosure(cl, 0, None)
    # argc, upvalc, [uctable], [gktable], [ftable]

def compile_closure(function, gktable, gk, uvtable):
    uctable = []
    vtable = function.argvar
    utable = []
    ftable = function.functions
    asm = Assembler()
    for nl in function.nonlocals.values():
        if nl.value not in gktable:
            uctable.append(uvtable.index(nl.value))
#                    runtime.boxFixnum(uvtable.index(nl.value)))
            utable.append(nl.value)
    #assert len(uctable) == 0
    #print "UC", uctable, utable
    for variable in function.variables:
        if variable.upvalue:
            utable.append(variable)
            if variable in vtable:
                asm.append( proxy.bcode("setupval", vtable.index(variable), utable.index(variable)) )
                #        runtime.abc("SETUPVAL", utable.index(variable), vtable.index(variable)) )
        elif variable not in vtable:
            vtable.append(variable)
    env = [vtable, utable, gktable, ftable, len(vtable)]
    tmp_regs = env[4]
    for block in function.blocks:
        asm.label(block)
        for stmt in block.statements:
            to_bytecode(asm, stmt, tmp_regs, env)
    asm.link()

    ftab = []
    for func in ftable:
        ftab.append(proxy.box_object(compile_closure(func, gktable, gk, utable)))

    #print "new closures", ftab, vtable
    bytecode = ''.join(asm.code)
    return proxy.build_descriptor(len(function.argvar), env[4], len(utable), uctable, bytecode, ftab, gk)
    #return runtime.buildClosureDef(env[4], len(function.argvar), len(utable), uctable, gk, ftab, bytecode)

#    vt = (runtime.Value*env[4])()
#
#    print env
#    print repr(bytecode)
#    print 'trying to interpret'
#    runtime.rti.interpret(bytecode, vt, gk, None)
#    print 'done'

binops = {
    '+': "add", '-': "sub", '*': "mul", '/': "div",
    '!=': "ne", '<>': "ne", '==': "eq",
    '>': "gt", '>=': "ge",
    '<': "lt", '<=': "le",
}

def to_bytecode(asm, stmt, dreg, env):
    vtable, utable, gktable, ftable, breg = env
    env[4] = max(breg, dreg+1)
    if isinstance(stmt, NonLocal):
        p = stmt.value
        if p in gktable:
            return asm.append(proxy.bcode('getconst', dreg, gktable.index(p)))
        else:
            return asm.append(proxy.bcode('getupval', dreg, utable.index(p)))
    elif isinstance(stmt, Value):
        return asm.append(proxy.bcode('getconst', dreg, gktable.index(stmt)))
    elif isinstance(stmt, Move):
        to_bytecode(asm, stmt.src, dreg, env)
        if stmt.dst in vtable:
            return asm.append(proxy.bcode('move', vtable.index(stmt.dst), dreg))
        else:
            return asm.append(proxy.bcode('setupval', dreg, utable.index(stmt.dst)))
    elif isinstance(stmt, Iter):
        to_bytecode(asm, stmt.src, dreg, env)
        return asm.append(proxy.bcode('iter', dreg, dreg))
    elif isinstance(stmt, Variable):
        if stmt in vtable:
            return asm.append(proxy.bcode('move', dreg, vtable.index(stmt)))
        else:
            return asm.append(proxy.bcode('getupval', dreg, utable.index(stmt)))
    elif isinstance(stmt, Test):
        to_bytecode(asm, stmt.cond, dreg, env)
        if stmt.inverse:
            return asm.append(proxy.bcode('testn', dreg, 0), link=stmt.then)
        else:
            return asm.append(proxy.bcode('test',  dreg, 0), link=stmt.then)
    elif isinstance(stmt, Jump):
        return asm.append(proxy.bcode('jump', stmt.drop, 0), link=stmt.then)
    elif isinstance(stmt, Return):
        to_bytecode(asm, stmt.value, dreg, env)
        return asm.append(proxy.bcode('return', dreg, 1))
    elif isinstance(stmt, Function):
        return asm.append(proxy.bcode('closure', dreg, ftable.index(stmt)))
    elif isinstance(stmt, Try):
        if stmt.exc != None:
            asm.append(proxy.bcode('except', 0, 0), link=stmt.exc)
        return
    elif isinstance(stmt, Next):
        asm.append(proxy.bcode('next', dreg, vtable.index(stmt.it)))
        asm.append(proxy.bcode('nextc', 0, 0), link=stmt.stop)
        return
    elif stmt is True:
        return asm.append(proxy.bcode('loadbool', dreg, 1))
    elif stmt is False:
        return asm.append(proxy.bcode('loadbool', dreg, 0))
    elif stmt is None:
        return asm.append(proxy.bcode('loadnull', dreg, 1))
    elif stmt.name == 'loop':
        return asm.append(proxy.bcode('loop'))
    elif stmt.name == 'call':
        for i, expr in enumerate(stmt.args):
            to_bytecode(asm, expr, dreg+i, env)
        return asm.append(proxy.bcode('call', dreg, dreg+len(stmt.args)))
    elif stmt.name == 'getattr':
        to_bytecode(asm, stmt.args[0], dreg+0, env)
        to_bytecode(asm, stmt.args[1], dreg+1, env)
        return asm.append(proxy.bcode('getattr', dreg))
    elif stmt.name == 'callattr':
        argc = len(stmt.args) - 1
        to_bytecode(asm, stmt.args[0], dreg+argc, env)
        for i, expr in enumerate(stmt.args[1:]):
            to_bytecode(asm, expr, dreg+i, env)
        return asm.append(proxy.bcode('callattr', dreg+argc, dreg, dreg+argc))
    elif stmt.name == 'setattr':
        to_bytecode(asm, stmt.args[0], dreg+0, env)
        to_bytecode(asm, stmt.args[1], dreg+1, env)
        to_bytecode(asm, stmt.args[2], dreg+2, env)
        return asm.append(proxy.bcode('setattr', dreg))
    elif stmt.name in binops:
        name = binops[stmt.name]
        i = 0
        args = []
        for arg in stmt.args:
            if arg in vtable:
                args.append(vtable.index(arg))
            else:
                to_bytecode(asm, arg, dreg+i, env)
                args.append(dreg+i)
                i+=1
        return asm.append(proxy.bcode(name, dreg, *args))
    elif stmt.name == 'getitem':
        to_bytecode(asm, stmt.args[0], dreg+0, env)
        to_bytecode(asm, stmt.args[1], dreg+1, env)
        return asm.append(proxy.bcode('getitem', dreg))
    elif stmt.name == 'setitem':
        to_bytecode(asm, stmt.args[0], dreg+0, env)
        to_bytecode(asm, stmt.args[1], dreg+1, env)
        to_bytecode(asm, stmt.args[2], dreg+2, env)
        return asm.append(proxy.bcode('setitem', dreg))
    raise Exception("cannot compile %r" % stmt)

class Assembler(object):
    def __init__(self):
        self.code   = []
        self.labels = {}
        self.links  = []

    def append(self, inst, link=None):
        assert len(inst) == 4
        if link is not None:
            self.links.append((len(self.code), link))
        self.code.append(inst)

    def label(self, block):
        self.labels[block] = len(self.code)

    def link(self):
        for loc, target in self.links:
            to = self.labels[target]
            self.code[loc] = proxy.bcode_adjust(self.code[loc], ((to - loc - 1) * 4) + 0x8000)

def compile_sentence_toplevel(builder, snt):
    if snt.group == 'infix' and snt[0].value == '=' and snt[1].group == 'symbol':
        variable = builder.function.get(snt[1].value)
        builder.append(Move(variable, compile_sentence(builder, snt[2])))
        return
    if snt.group == 'infix' and snt[0].value == '=' and snt[1].group == 'attribute':
        subject = compile_expression(builder, snt[1][0])
        attr    = builder.const(snt[1][1].value)
        builder.append(Operation('setattr', [subject, attr, compile_sentence(builder, snt[2])]))
        return

    assert groupe(snt, 'sentence') and len(snt) > 0
    if symbole(snt[0], 'if') and snt[2].group == 'block':
        cond = compile_expression(builder, snt[1])
        then = builder.new_block()
        exit = builder.new_block()
        builder.test(cond, then)
        otherwise = builder.block
        builder.block = then
        for node in snt[2]:
            compile_sentence_toplevel(builder, node)
        builder.goto(exit, otherwise)
        builder.otherwise_exit = exit
        return
    if symbole(snt[0], 'elif') and snt[2].group == 'block' and builder.otherwise_exit is not None:
        otherwise = builder.block
        then = builder.new_block()
        exit = builder.otherwise_exit
        builder.otherwise_exit = None
        cond = compile_expression(builder, snt[1])
        builder.test(cond, then)
        builder.block = then
        for node in snt[2]:
            compile_sentence_toplevel(builder, node)
        builder.goto(exit, otherwise)
        builder.otherwise_exit = exit
        return
    if symbole(snt[0], 'else') and snt[1].group == 'block' and builder.otherwise_exit is not None:
        exit = builder.otherwise_exit
        builder.otherwise_exit = None
        for node in snt[1]:
            compile_sentence_toplevel(builder, node)
        builder.goto(exit, exit)
        return
    if symbole(snt[0], 'while') and snt[2].group == 'block':
        loop = builder.new_block()
        exit = builder.new_block()
        builder.goto(loop, loop)
        builder.append(Operation('loop', []))
        builder.loops.append((loop, exit))
        cond = compile_expression(builder, snt[1])
        builder.test(cond, exit, inverse=True)
        for node in snt[2]:
            compile_sentence_toplevel(builder, node)
        builder.goto(loop, exit)
        builder.loops.pop(-1)
        return
    if symbole(snt[0], 'for') and symbole(snt[2], 'in'):
        loop = builder.new_block()
        exit = builder.new_block()
        var = builder.function.get(snt[1].value)
        it  = builder.function.new_var()
        builder.append(Move(it, Iter(compile_expression(builder, snt[3]))))
        builder.loops.append((loop, exit))
        builder.goto(loop, loop)
        builder.append(Move(var, Next(it, exit)))
        for node in snt[4]:
            compile_sentence_toplevel(builder, node)
        builder.goto(loop, exit)
        builder.loops.pop(-1)
        return
    if symbole(snt[0], 'once') and symbole(snt[2], 'in'):
        exit = builder.new_block()
        otherwise = builder.new_block()
        var = builder.function.get(snt[1].value)
        it  = builder.function.new_var()
        builder.append(Move(it, Iter(compile_expression(builder, snt[3]))))
        builder.append(Move(var, Next(it, exit)))
        for node in snt[4]:
            compile_sentence_toplevel(builder, node)
        builder.goto(exit, otherwise)
        builder.otherwise_exit = exit
        return
    if len(builder.loops) > 0 and symbole(snt[0], 'continue'):
        builder.goto(builder.loops[-1][0], None)
    if len(builder.loops) > 0 and symbole(snt[0], 'break'):
        builder.goto(builder.loops[-1][1], None)
    if symbole(snt[0], 'return'):
        builder.ret(compile_expression(builder, snt[1]))
        return
    if symbole(snt[0], 'try'):
        exit = builder.trying()
        for node in snt[-1]:
            compile_sentence_toplevel(builder, node)
        builder.donetrying()
        builder.goto(exit, exit, drop=1)
        return
    if symbole(snt[0], 'except'):
        exit = builder.except_block()
        for node in snt[-1]:
            compile_sentence_toplevel(builder, node)
        builder.goto(exit, exit)
        return
    builder.append(compile_sentence(builder, snt))

def compile_sentence(builder, snt):
    if len(snt) == 1:
        return compile_expression(builder, snt[0])
    if len(snt) == 2:
        callee, args = snt
        if callee.group == 'attribute':
            opname = 'callattr'
            if callee[1].group == 'attribute':
                 subject = compile_expression(builder, callee[0])
                 attr    = builder.const(callee[1].value)
                 prefix = [subject, attr]
        else:
            callee = compile_expression(builder, callee)
            prefix = [callee]
            opname = 'call'
        if groupe(args, 'tuple+'):
            args = [compile_expression(builder, expr) for expr in args]
        else:
            args = [compile_expression(builder, args)]
        return Operation(opname, prefix + args)
    raise Exception("%s: cannot compile %r" % (parser.linecol(snt.location), snt))

def compile_expression(builder, expr):
    if symbole(expr, 'null'):
        return None
    if symbole(expr, 'true'):
        return True
    if symbole(expr, 'false'):
        return False
    if expr.group == 'symbol':
        return builder.function.lookup(expr.value)
    if expr.group == 'string':
        return builder.const(expr.value)
    if expr.group == 'number':
        return builder.const(int(expr.value))
    if expr.group == 'float':
        return builder.const(float(expr.value))
    if expr.group == 'call':
        callee = expr[0]
        args   = expr[1:]
        if callee.group == 'attribute':
            opname = 'callattr'
            if callee[1].group == 'attribute':
                 subject = compile_expression(builder, callee[0])
                 attr    = builder.const(callee[1].value)
                 prefix = [subject, attr]
        else:
            callee = compile_expression(builder, callee)
            prefix = [callee]
            opname = 'call'
        args = [compile_expression(builder, e) for e in args]
        return Operation(opname, prefix + args)
    if expr.group == 'infix':
        op, lhs, rhs = expr
        lhs = compile_expression(builder, lhs)
        rhs = compile_expression(builder, rhs)
        return Operation(op.value, [lhs, rhs])
    if expr.group == 'function':
        args = []
        for symbol in expr[:-1]:
            assert symbol.group == 'symbol'
            args.append(symbol.value)
        func = compile_function(builder.values, args, expr[-1])
        builder.function.functions.append(func)
        return func
    if expr.group == 'attribute' and expr[1].group == 'attribute':
        subject, name = expr
        subject = compile_expression(builder, subject)
        return Operation('getattr', [subject, builder.const(name.value)])
    if expr.group == 'index':
        subject = compile_expression(builder, expr[0])
        index   = compile_expression(builder, expr[1])
        return Operation('getitem', [subject, index])
    raise Exception("%s: cannot compile %r" % (parser.linecol(expr.location), expr))


def compile_function(const, args, body):
    builder = Build(const, Function())
    for name in args:
        builder.function.argvar.append( builder.function.get(name) )
    for sentence in body:
        compile_sentence_toplevel(builder, sentence)
    builder.ret(None)
    for func in builder.function.functions:
        for nl in func.nonlocals.values():
            nl.link = v = builder.function.lookup(nl.name)
            v.upvalue = True
    return builder.function

def groupe(node, group):
    return node.group == group

def symbole(node, name):
    return node.group == 'symbol' and node.value == name

class Build(object):
    def __init__(self, values, function):
        self.values = values
        self.function = function
        self.block = self.new_block()
        self.otherwise_exit = None
        self.loops = []
        self.try_block = None
        self.try_post_block = None

    def new_block(self):
        block = Block()
        self.function.blocks.append(block)
        return block

    def const(self, value):
        if value in self.values:
            ret = self.values[value]
        else:
            ret = self.values[value] = Value(value)
        return ret

    def get_global(self, name):
        if name in self.globals:
            ret = self.globals[name]
        else:
            ret = self.globals[name] = Variable(None, name)
        return ret

    def append(self, stmt):
        if self.otherwise_exit is not None:
            then = self.otherwise_exit
            self.otherwise_exit = None
            self.goto(then, then)
        if self.block is not None:
            self.block.append(stmt)

    def test(self, cond, then, inverse=False):
        self.append(Test(cond, then, inverse))

    def goto(self, then, nxt=None, drop=0):
        self.append(Jump(then, drop))
        self.block = nxt

    def ret(self, value, nxt=None):
        self.append(Return(value))
        self.block = nxt

    def trying(self):
        exit = self.new_block()
        tr = Try()
        self.append(tr)
        self.try_block = [tr, exit]
        return exit

    def donetrying(self):
        self.try_post_block = self.try_block
        self.try_block = None

    def except_block(self):
        exc = self.try_post_block[0].exc = self.new_block()
        self.block = exc
        return self.try_post_block[1]

class Function(object):
    def __init__(self):
        self.argvar = []
        self.variables = []
        self.binds     = {}
        self.nonlocals = {}
        self.blocks = []
        self.functions = []

    def get(self, name):
        if name in self.binds:
            variable = self.binds[name]
        else:
            variable = self.binds[name] = self.new_var(name)
        return variable

    def lookup(self, name):
        if name in self.binds:
            return self.binds[name]
        elif name in self.nonlocals:
            nl = self.nonlocals[name]
        else:
            nl = self.nonlocals[name] = NonLocal(name)
        return nl

    def new_var(self, name=None):
        var = Variable(self, name)
        self.variables.append(var)
        return var

class Block(object):
    def __init__(self):
        self.statements = []
        self.name = 'block_' + str(id(self))

    def append(self, stmt):
        self.statements.append(stmt)

    def __repr__(self):
        return self.name

class Operation(object):
    def __init__(self, name, args):
        self.name = name
        self.args = args

    def __repr__(self):
        return self.name + '(' + ', '.join(map(repr, self.args)) + ')'

class Move(object):
    def __init__(self, dst, src):
        self.dst = dst
        self.src = src 

    def __repr__(self):
        return repr(self.dst) + ' = ' + repr(self.src)

class Test(object):
    def __init__(self, cond, then, inverse=False):
        self.cond = cond
        self.then = then
        self.inverse = inverse

    def __repr__(self):
        return 'test ' + repr(self.cond) + ' ' + repr(self.then)

class Jump(object):
    def __init__(self, then, drop=0):
        self.then = then
        self.drop = drop

    def __repr__(self):
        return 'jump ' + repr(self.then)

class Return(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return 'return ' + repr(self.value)

class Try(object):
    def __init__(self, exc=None):
        self.exc = exc

class Iter(object):
    def __init__(self, src):
        self.src = src

class Next(object):
    def __init__(self, it, stop=None):
        self.it   = it
        self.stop = stop

class Variable(object):
    def __init__(self, function, name):
        self.function = function
        self.name = name
        self.upvalue = False

    def __repr__(self):
        return self.name

class NonLocal(object):
    def __init__(self, name):
        self.name = name
        self.link = None
        self.upvalue = False

    @property
    def value(self):
        if isinstance(self.link, NonLocal):
            return self.link.value
        elif self.link is None:
            return self
        else:
            return self.link

    def __repr__(self):
        return 'nonlocal(%s)' % self.name

class Value(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return '{:' + repr(self.value) + ':}'

if __name__=='__main__':
    main()
