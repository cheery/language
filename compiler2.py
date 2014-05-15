#!/usr/bin/env python2.7
import argparse
import parser
import runtime

def main():
    argparser = argparse.ArgumentParser(description="Compile file to bytecode")
    argparser.add_argument('filename', type=str, help="File to compile")
    args = argparser.parse_args()

    builder = Build({}, Function())
    for sentence in parser.parse_file(args.filename):
        compile_sentence_toplevel(builder, sentence)
    builder.ret(None)

    function = builder.function

    gktable = function.nonlocals.values()
    gktable.extend(builder.values.values())

    vtable = function.argvar
    for variable in function.variables.values():
        if variable not in vtable:
            vtable.append(variable)

    env = [vtable, [], gktable, len(vtable)]

    tmp_regs = env[3]
    asm = Assembler()
    for block in function.blocks:
        asm.label(block)
        for stmt in block.statements:
            to_bytecode(asm, stmt, tmp_regs, env)
    asm.link()

    bytecode = ''.join(asm.code)
    vt = (runtime.Value*env[3])()
    gk = (runtime.Value*len(gktable))()
    for i, item in enumerate(gktable):
        if isinstance(item, Value):
            if isinstance(item.value, (unicode, str)):
                s = item.value.encode('utf-8')
                gk[i] = runtime.boxString(len(s), s)
            elif isinstance(item.value, (int, long)):
                gk[i] = runtime.boxFixnum(item.value)
            else:
                raise Exception("um? %r" % item.value)
        else:
            gk[i] = runtime.sysfuncs[item.name]

    print env
    print repr(bytecode)
    print 'trying to interpret'
    runtime.rti.interpret(bytecode, vt, gk, None)
    print 'done'

binops = {
    '+': "add", '-': "sub", '*': "mul", '/': "div",
    '!=': "ne", '<>': "ne", '==': "eq",
    '>': "gt", '>=': "ge",
    '<': "lt", '<=': "le",
}

def to_bytecode(asm, stmt, dreg, env):
    vtable, utable, gktable, breg = env
    env[3] = max(breg, dreg+1)
    if isinstance(stmt, NonLocal):
        p = stmt.value
        if p in gktable:
            return asm.append(runtime.ad('LOADGK', dreg, gktable.index(p)))
    elif isinstance(stmt, Value):
        return asm.append(runtime.ad('LOADGK', dreg, gktable.index(stmt)))
    elif isinstance(stmt, Move):
        to_bytecode(asm, stmt.src, dreg, env)
        return asm.append(runtime.abc('MOVE', vtable.index(stmt.dst), dreg))
    elif isinstance(stmt, Variable):
        return asm.append(runtime.abc('MOVE', dreg, vtable.index(stmt)))
    elif isinstance(stmt, Test):
        to_bytecode(asm, stmt.cond, dreg, env)
        if stmt.inverse:
            return asm.append(runtime.ad('TESTN', dreg), link=stmt.then)
        else:
            return asm.append(runtime.ad('TEST',  dreg), link=stmt.then)
    elif isinstance(stmt, Jump):
        return asm.append(runtime.ad('JUMP'), link=stmt.then)
    elif isinstance(stmt, Return):
        to_bytecode(asm, stmt.value, dreg, env)
        return asm.append(runtime.abc('RETURN', dreg, 1))
    elif stmt is True:
        return asm.append(runtime.ad('LOADTRUE', dreg))
    elif stmt is False:
        return asm.append(runtime.ad('LOADFALSE', dreg))
    elif stmt is None:
        return asm.append(runtime.ad('LOADNULL', dreg, 1))
    elif stmt.name == 'loop':
        return asm.append(runtime.ad('LOOP'))
    elif stmt.name == 'call':
        for i, expr in enumerate(stmt.args):
            to_bytecode(asm, expr, dreg+i, env)
        return asm.append(runtime.abc('call', dreg, dreg, len(stmt.args)))
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
        return asm.append(runtime.abc(name, dreg, *args))

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
            self.code[loc] = runtime.rewrite_d(self.code[loc], ((to - loc - 1) * 4) + 0x8000)

def compile_sentence_toplevel(builder, snt):
    if snt.group == 'infix' and snt[0].value == '=' and snt[1].group == 'symbol':
        variable = builder.function.get(snt[1].value)
        builder.append(Move(variable, compile_sentence(builder, snt[2])))
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
    if len(builder.loops) > 0 and symbole(snt[0], 'continue'):
        builder.goto(builder.loops[-1][0], None)
    if len(builder.loops) > 0 and symbole(snt[0], 'break'):
        builder.goto(builder.loops[-1][1], None)
    builder.append(compile_sentence(builder, snt))

def compile_sentence(builder, snt):
    if len(snt) == 1:
        return compile_expression(builder, snt[0])
    if len(snt) == 2:
        callee, args = snt
        callee = compile_expression(builder, callee)
        if groupe(args, 'tuple+'):
            args = [compile_expression(builder, expr) for expr in args]
        else:
            args = [compile_expression(builder, args)]
        return Operation('call', [callee] + args)
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
    if expr.group == 'call':
        args = [compile_expression(builder, e) for e in expr]
        return Operation('call', args)
    if expr.group == 'infix':
        op, lhs, rhs = expr
        lhs = compile_expression(builder, lhs)
        rhs = compile_expression(builder, rhs)
        return Operation(op.value, [lhs, rhs])
    raise Exception("%s: cannot compile %r" % (parser.linecol(expr.location), expr))

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

    def goto(self, then, nxt=None):
        self.append(Jump(then))
        self.block = nxt

    def ret(self, value, nxt=None):
        self.append(Return(value))
        self.block = nxt

class Function(object):
    def __init__(self):
        self.argvar = []
        self.variables = {}
        self.nonlocals = {}
        self.blocks = []
        self.functions = []

    def get(self, name):
        if name in self.variables:
            variable = self.variables[name]
        else:
            variable = self.variables[name] = Variable(self, name)
        return variable

    def lookup(self, name):
        if name in self.variables:
            return self.variables[name]
        elif name in self.nonlocals:
            nl = self.nonlocals[name]
        else:
            nl = self.nonlocals[name] = NonLocal(name)
        return nl

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
    def __init__(self, then):
        self.then = then

    def __repr__(self):
        return 'jump ' + repr(self.then)

class Return(object):
    def __init__(self, value):
        self.value = value

    def __repr__(self):
        return 'return ' + repr(self.value)

class Variable(object):
    def __init__(self, function, name):
        self.function = function
        self.name = name

    def __repr__(self):
        return self.name

class NonLocal(object):
    def __init__(self, name):
        self.name = name
        self.link = None

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
