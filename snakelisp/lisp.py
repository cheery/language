import parser
import transpiler
from cps import Call, Lambda, Assign, Variable, Constant, Environ, null, true, false
import subprocess

# call = Call([arguments]), call[i]
# lambda = Lambda([arguments], body), lambda[i]
# Assign(var, val, body)
# Variable(name, value)
# Constant(value)

def main():
    mks = []
    env = Environ()
    ret = env.new_argument("cont", False)
    var = null
    for expr in open_list('demo'):
        var = continuate(mks, expr, env)
    program = env.close(compose(mks, Call([ret, var])))
    program = program.coalesce()

    c_api = {
        "pick":        "&v_pick",
        "array":       "&v_array",
        "arraybuffer": "&v_arraybuffer",
        "file-open":   "&v_file_open",
        "file-close":  "&v_file_close",
        "file-read":   "&v_file_read",
        "file-write":  "&v_file_write",
        "stdin":       "&v_stdin",
        "stdout":      "&v_stdout",
        "stderr":      "&v_stderr",
        "cat":         "&v_cat",
        "length":      "&v_length",
        "idx":         "&v_load_idx",
        "idx=":        "&v_store_idx",
        "closure?":    "&v_is_closure",
        "null?":       "&v_is_null",
        "true?":       "&v_is_true",
        "false?":      "&v_is_false",
        "boolean?":    "&v_is_boolean",
        "integer?":    "&v_is_integer",
        "double?":     "&v_is_double",
        "array?":      "&v_is_array",
        "arraybuffer?":"&v_is_arraybuffer",
        "string?":     "&v_is_string",
    }
    cdefns = ["extern value_t {};".format(value[1:]) for name, value in c_api.items()]
    for var in env.seal():
        var.c_handle = c_api[var.name]
    source = transpiler.transpile(program, cdefns, "demo")
    open('demo.c', 'w').write(source)
    subprocess.call(["gcc", "demo.c", "snakelisp.c"])


constants = {'null': null, 'true':true, 'false':false}
def continuate(mks, expr, env):
    if ismacro2(expr, '=') and expr[0].group == 'symbol':
        var = env.get_local(expr[0].value)
        val = continuate(mks, expr[2], env)
        mks.append(lambda cont: Assign(var, val, cont))
        return val
    if ismacro2(expr, ':=') and expr[0].group == 'symbol':
        var = env.lookup(expr[0].value)
        val = continuate(mks, expr[2], env)
        mks.append(lambda cont: Assign(var, val, cont))
        return val
    if ismacro(expr, 'if'):
        _, cond, tru, fal = expr
        cond = continuate(mks, cond, env)
        a = enclose([tru], env)
        b = enclose([fal], env)
        return genpick(mks, cond, a, b, env)
    if ismacro(expr, 'cond'):
        return build_cond(mks, expr[1:], env)
    if ismacro(expr, 'func'):
        env = env.new_environ()
        ret = env.new_argument('cont', False)
        smks = []
        for sym in expr[1]:
            assert sym.group == 'symbol'
            env.new_argument(sym.value)
        var = null
        for subexpr in expr[2:]:
            var = continuate(smks, subexpr, env)
        return env.close(compose(smks, Call([ret, var])))
    if expr.group == 'list':
        arglist = [continuate(mks, a, env) for a in expr]
        callee  = arglist.pop(0)
        retval  = Variable()
        mks.append(lambda cont: Call([callee, Lambda([retval], cont)] + arglist))
        return retval
    if expr.group == 'symbol':
        if expr.value in constants:
            return constants[expr.value]
        return env.lookup(expr.value)
    if expr.group == 'integer':
        return Constant(expr.value)
    if expr.group == 'double':
        return Constant(expr.value)
    if expr.group == 'string':
        return Constant(expr.value)
    raise Exception("what is {}?".format(expr))

def build_cond(mks, exprs, env):
    retval = Variable()
    if len(exprs) == 0:
        mks.append(lambda cont: Call([Lambda([retval], cont), null]))
    elif len(exprs) == 1 and ismacro(exprs[0], 'else'):
        val = null
        for subexpr in exprs[0][1:]:
            val  = continuate(mks, subexpr, env)
        mks.append(lambda cont: Call([Lambda([retval], cont), val]))
    else:
        cond = exprs[0][0]
        body = exprs[0][1:]
        cond = continuate(mks, cond, env)
        a    = enclose(body, env)

        cmks = []
        cont = Variable()
        val  = build_cond(cmks, exprs[1:], env)
        b    = Lambda([cont], compose(cmks, Call([cont, val])))
        return genpick(mks, cond, a, b, env)
    return retval

def enclose(exprs, env):
    cont = Variable()
    mks = []
    val = null
    for expr in exprs:
        val = continuate(mks, expr, env)
    return Lambda([cont], compose(mks, Call([cont, val])))

def genpick(mks, cond, a, b, env):
    retval = Variable()
    mks.append(lambda cont: Call([env.new_implicit('pick'), Lambda([retval], cont), cond, a, b]))
    return retval

def ismacro(expr, name):
    return expr.group == 'list' and len(expr) > 0 and expr[0].value == name

def ismacro2(expr, name):
    return expr.group == 'list' and len(expr) > 2 and expr[1].value == name

def open_list(path):
    with open(path, 'r') as fd:
        return parser.parse(fd.read())

def compose(mks, cont):
    for fn in reversed(mks):
        cont = fn(cont)
    return cont

if __name__ == '__main__':
    main()
