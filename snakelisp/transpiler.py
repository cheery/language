from cps import Call, Lambda, Assign, Variable, Constant, Environ, null, true, false

def transpile(lamb):
    ctx  = Context()
    lambdas = collect_lambdas(set(), lamb)
    scopevars = {}
    collect_scopevars(scopevars, lamb)
    lines = ['#include "snakelisp.h"', ""]
    indent = 0 
    def dump(fmt, *args):
        if len(args):
            lines.append(" "*indent + fmt.format(*args))
        else:
            lines.append(" "*indent + fmt)
    for func in lambdas:
        dump("static void {}(closure_t*, argv_t*) c_noreturn;", ctx.fname[func])
        func.env = {}
        func.upscope = list(scopevars[func])
        for i, var in enumerate(func.upscope):
            func.env[var] = "c_slot(closure, {})".format(i)

    dump("int main(int argc, char** argv)")
    dump("{")
    indent += 2
    dump("c_boot({});", ctx.fname[lamb])
    dump("return 0;")
    indent -= 2
    dump("}")
    
    for func in lambdas:
        dump("")
        dump("static void {}(closure_t* closure, argv_t* args)", ctx.fname[func])
        dump("{")
        indent += 2

        for i, arg in enumerate(func):
            func.env[arg] = "&a{}".format(i+1)
            dump("value_t a{0} = c_get_argument(args, {0});", i+1)

        constants = set(scrape_constants(func))
        for i, const in enumerate(constants):
            name = "c{}".format(i)
            func.env[const] = '&'+name
            if const is None:
                dump("value_t {} = c_const_null();", name)
            elif const is True:
                dump("value_t {} = c_const_true();", name)
            elif const is False:
                dump("value_t {} = c_const_false();", name)
            elif isinstance(const, (int, long)):
                dump("value_t {} = c_const_integer({});", name, const)
            else:
                raise Exception("what is this? {}".format(const))

        closures = set(scrape_closures(func))
        for arg in closures:
            name  = ctx.fname[arg]
            cname = 'c'+name
            vname = 'v'+name
            dump("c_alloc_closure({}, {}, {}, {});", name, cname, vname, len(arg.upscope))
            func.env[arg] = '&'+vname
        varnames = []
        for var in set(var for var, value in func.motion):
            if var in func.upscope:
                continue
            name = ctx.vname[var]
            func.env[var] = '&'+name
            varnames.append(name)
        if varnames:
            dump("value_t {};", ', '.join(varnames))
        for var, value in func.motion:
            dump("*{} = *{};", as_argument(ctx, func, var), as_argument(ctx, func, value))

        dump("c_call_begin({});", len(func.body))
        for arg in closures:
            name = 'c'+ctx.fname[arg]
            for i, var in enumerate(arg.upscope):
                dump("c_closure_lift({}, {}, {});".format(name, i, as_argument(ctx, func, var)))
        for i, arg in enumerate(func.body):
            dump("c_call_argument({}, *{});", i, as_argument(ctx, func, arg))
        dump("c_call_end();")

        indent = 0
        dump("}")
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
        if obj in scopevars:
            return scopevars[obj]
        inscope = collect_scopevars(scopevars, obj.body)
        for var, val in reversed(obj.motion):
            inscope.discard(var)
            inscope |= collect_scopevars(scopevars, val)
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
        return func.env.get(arg, arg.name)
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
