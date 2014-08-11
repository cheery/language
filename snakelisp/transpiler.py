from cps import Call, Lambda, Assign, Variable, Constant, Environ, null, true, false

def transpile(lamb, extra_headers=(), sourcename="<noname>", debug=False):
    ctx  = Context()
    lambdas = collect_lambdas(set(), lamb)
    scopevars = {}
    collect_scopevars(scopevars, lamb)
    lines = [
        '/* generated from: {} */'.format(sourcename),
        '#include "snakelisp.h"',
        ""]
    if debug:
        lines.append("#include <stdio.h>")
    lines.extend(extra_headers)
    indent = 0 
    def dump(fmt, *args):
        if len(args):
            lines.append(" "*indent + fmt.format(*args))
        else:
            lines.append(" "*indent + fmt)
    for func in lambdas:
        dump("static CONTINUATION({});", ctx.fname[func])
        func.env = {}
        func.upscope = list(scopevars[func])
        for i, var in enumerate(func.upscope):
            func.env[var] = "SLOT({})".format(i)

    dump("void main(int argc, char** argv)")
    dump("{")
    indent += 2
    dump("snakeBoot(spawnClosure({}));", ctx.fname[lamb])
    indent -= 2
    dump("}")
    
    for func in lambdas:
        dump("")
        dump("CONTINUATION({})", ctx.fname[func])
        dump("{")
        indent += 2

        for i, arg in enumerate(func):
            func.env[arg] = "&a{}".format(i+1)
            dump("value_t a{0} = ARG({0});", i+1)

        if debug:
            dump('fprintf(stderr, ":{}\\n");', ctx.fname[func])

        constants = set(scrape_constants(func))
        for i, const in enumerate(constants):
            name = "c{}".format(i)
            func.env[const] = '&'+name
            if const is None:
                dump("value_t {} = boxNull();", name)
            elif const is True:
                dump("value_t {} = boxTrue();", name)
            elif const is False:
                dump("value_t {} = boxFalse();", name)
            elif isinstance(const, (int, long)):
                dump("value_t {} = boxInteger({});", name, const)
            elif isinstance(const, float):
                dump("value_t {} = boxDouble({});", name, const)
            elif isinstance(const, (str, unicode)):
                dump("value_t {} = spawnString({});", name, const)
            else:
                raise Exception("what is this? {}".format(const))

        closures = set(scrape_closures(func))
        varnames = []
        for var in set(var for var, value in func.motion):
            if var in func.upscope:
                continue
            name = ctx.vname[var]
            func.env[var] = '&'+name
            varnames.append(name)
        if varnames:
            dump("value_t {};", ', '.join(varnames))
        for arg in closures:
            name  = ctx.fname[arg]
            vname = 'v'+name
            dump("value_t {} = spawnClosure({}, {});", vname, name, ', '.join(
                as_argument(ctx, func, arg)
                for arg in arg.upscope))
            func.env[arg] = '&'+vname
        for var, value in func.motion:
            dump("*{} = *{};", as_argument(ctx, func, var), as_argument(ctx, func, value))
        dump("call({});", ", ".join(
            "*" + as_argument(ctx, func, arg)
            for arg in func.body))
        indent = 0
        dump("}")

    if len(lamb.upscope):
        raise Exception("toplevel has unknown upscope variables: {}".format(lamb.upscope))
    return '\n'.join(lines) + '\n'

def collect_lambdas(lambdas, obj):
    if obj.type == 'lambda':
        lambdas.add(obj)
        for variable, value in obj.motion:
            collect_lambdas(lambdas, value)
        collect_lambdas(lambdas, obj.body)
    elif obj.type == 'call':
        for expr in obj:
            collect_lambdas(lambdas, expr)
    return lambdas

def collect_scopevars(scopevars, obj):
    if obj.type == 'lambda':
        inscope = collect_scopevars(scopevars, obj.body)
        for mot in reversed(obj.motion):
            var, val = mot
            inscope |= collect_scopevars(scopevars, val)
            if var.glob:
                continue
            if mot in obj.notdefn:
                inscope.add(var)
            else:
                inscope.discard(var)
        for var in obj:
            inscope.discard(var)
        scopevars[obj] = inscope
        return inscope
    elif obj.type == 'call':
        inscope = set()
        for expr in obj:
            inscope |= collect_scopevars(scopevars, expr)
        return inscope
    elif obj.type == 'variable' and not obj.glob:
        return {obj}
    return set()

def scrape_constants(lamb):
    for var, val in lamb.motion:
        if val.type == 'constant':
            yield val.value
    for val in lamb.body:
        if val.type == 'constant':
            yield val.value

def scrape_closures(func):
    for var, val in func.motion:
        if val.type == 'lambda':
            yield val
    for arg in func.body:
        if arg.type == 'lambda':
            yield arg

def as_argument(ctx, func, arg):
    if arg.type == 'variable':
        if arg.glob:
            return arg.c_handle
        if arg.name:
            return func.env[arg] + "/*" + arg.name + "*/"
        else:
            return func.env[arg]
    if arg.type == 'constant':
        return func.env[arg.value]
    if arg.type == 'lambda':
        return func.env[arg]
    raise Exception("unknown cps argument {}".format(arg))

class Context(object):
    def __init__(self):
        self.fname = NameGen("f{}")
        self.vname = NameGen("v{}")
        self.cname = NameGen("c{}")

class NameGen(object):
    def __init__(self, template):
        self.template = template
        self.next_id  = 101
        self.cache    = {}

    def __getitem__(self, obj):
        if obj in self.cache:
            return self.cache[obj]
        name = self.cache[obj] = self.template.format(self.next_id)
        self.next_id += 1
        return name
