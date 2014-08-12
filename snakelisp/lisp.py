#!/usr/bin/env python
import parser
import transpiler
from cps import Call, Lambda, Assign, Variable, Constant, Environ, null, true, false
import subprocess
import sys

# call = Call([arguments]), call[i]
# lambda = Lambda([arguments], body), lambda[i]
# Assign(var, val, body)
# Variable(name, value)
# Constant(value)

def main():
    path = sys.argv[1]
    mks = []
    env = Environ()
    ret = env.new_argument("cont", False)

    env = env.new_environ()
    ret = env.new_argument('cont', False)
    exprs = open_list(path)
    program = env.close(compile_list(exprs, env, ret))
    program = program.coalesce()

    c_api = {
        "call/cc":     "&v_call_cc",
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
        "lt":          "&v_lt",
        "gt":          "&v_gt",
        "le":          "&v_le",
        "ge":          "&v_ge",
        "eq":          "&v_eq",
        "ne":          "&v_ne",
        "+":           "&v_add",
        "-":           "&v_sub",
        "*":           "&v_mul",
        "/":           "&v_div",
        "//":          "&v_floordiv",
        "%":           "&v_modulus",
        "chr":         "&v_to_character",
        "ord":         "&v_to_ordinal",
        "and":         "&v_and",
        "or":          "&v_or",
        "not":         "&v_not",
        "<<":          "&v_lsh",
        ">>":          "&v_rsh",
        "|":           "&v_bit_or",
        "&":           "&v_bit_and",
        "^":           "&v_bit_xor",
        "~":           "&v_bit_not",
        "log":         "&v_log",
        "exp":         "&v_exp",
        "pow":         "&v_pow",
        "sqrt":        "&v_sqrt",
        "uncallable-hook": "&uncallable_hook",
        "type-error-hook": "&type_error_hook",
        "interface":   "&v_get_interface",
        "interface=":  "&v_set_interface",
        "closure-interface": "&v_closure_interface",
        "numeric-interface": "&v_numeric_interface",
        "string-interface": "&v_string_interface",
        "arraybuffer-interface": "&v_arraybuffer_interface",
    }
    c_use = set()
    for var in env.seal():
        var.c_handle = c_api[var.name]
        c_use.add(var.c_handle)
    cdefns = ["extern value_t {};".format(value[1:]) for value in c_use]

    #import visuals
    #visuals.create_graph("demo.png", program)

    source = transpiler.transpile(program, cdefns, path)
    open(path+'.c', 'w').write(source)
    subprocess.call(["gcc", path+'.c', "snakelisp.c", "-I.", "-lm"])

constants = {'null': null, 'true':true, 'false':false}

def compile(expr, env, k):
    if ismacro2(expr, '=') and expr[0].group == 'symbol':
        var = env.get_local(expr[0].value)
        return compile(expr[2], env, 
            (lambda val: Assign(var, val, retrieve(k, val))))
    if ismacro2(expr, ':=') and expr[0].group == 'symbol':
        var = env.lookup(expr[0].value)
        return compile(expr[2], env, 
            (lambda val: Assign(var, val, retrieve(k, val))))
    if ismacro(expr, 'if'):
        _, cond, tru, fal = expr
        return compile(cond, env,
            (lambda truth: pick(env, k, truth,
                enclose([tru], env),
                enclose([fal], env))))
    if ismacro(expr, 'cond'):
        return compile_cond(expr, env, k)
    if ismacro(expr, 'while'):
        return compile_while(expr, env, k)
    if ismacro(expr, 'func'):
        env = env.new_environ()
        ret = env.new_argument('cont', False)
        for sym in expr[1]:
            assert sym.group == 'symbol'
            env.new_argument(sym.value)
        return retrieve(k, env.close(compile_list(expr[2:], env, ret)))
    if expr.group == 'list':
        params = []
        seq = list(expr)
        def next_parameter(param):
            params.append(param)
            if len(seq) > 0:
                return compile(seq.pop(0), env, next_parameter)
            else:
                callee = params.pop(0)
                return Call([callee, lift(k)] + params)
        return compile(seq.pop(0), env, next_parameter)
    if expr.group == 'symbol':
        if expr.value in constants:
            param = constants[expr.value]
        else:
            param = env.lookup(expr.value)
        return retrieve(k, param)
    if expr.group == 'integer':
        return retrieve(k, Constant(expr.value))
    if expr.group == 'double':
        return retrieve(k, Constant(expr.value))
    if expr.group == 'string':
        return retrieve(k, Constant(expr.value))
    raise Exception("what is {}?".format(expr))

def compile_list(exprs, env, k):
    seq = list(exprs)
    def next_parameter(param):
        if len(seq) > 1:
            return compile(seq.pop(0), env, next_parameter)
        else:
            return compile(seq.pop(0), env, k)
    if len(exprs) == 0:
        return retrieve(k, null)
    return next_parameter(null)

def retrieve(k, param):
    if callable(k):
        return k(param)
    else:
        return Call([k, param])

def lift(k):
    if callable(k):
        x = Variable()
        return Lambda([x], k(x))
    else:
        return k

def compile_cond(expr, env, k):
    seq = list(expr[1:])
    if len(seq) == 0:
        return retrieve(k, null)
    def next_cond(k):
        head = seq.pop(0)
        if len(seq) == 0 and ismacro(head, 'else'):
            return compile_list(head[1:], env, k)
        if len(seq) == 0:
            raise Exception("invalid cond expression")
        return compile(head[0], env,
            (lambda truth: pick(env, k, truth,
                enclose(head[1:], env),
                lambdaCont(next_cond))))
    return next_cond(k)

def compile_while(expr, env, k):
    self = Variable()
    seq  = expr[2:]
    def compile_body(k):
        return compile_list(expr[2:], env, (lambda _: Call([self, lift(k)])))
    cont = Variable()
    looplambda = Lambda([cont], compile(expr[1], env,
        (lambda truth: pick(env, cont, truth, lambdaCont(compile_body), lambdaNull()))))
    return Assign(self, looplambda, Call([self, lift(k)]), True)

def pick(env, k, truth, yes, no):
    return Call([env.new_implicit('pick'), lift(k), truth, yes, no])

def lambdaNull():
    cont = Variable()
    return Lambda([cont], Call([cont, null]))

def lambdaCont(func):
    cont = Variable()
    return Lambda([cont], func(cont))

def enclose(exprs, env):
    cont = Variable()
    return Lambda([cont], compile_list(exprs, env, cont))


def ismacro(expr, name):
    return expr.group == 'list' and len(expr) > 0 and expr[0].value == name

def ismacro2(expr, name):
    return expr.group == 'list' and len(expr) > 2 and expr[1].value == name

def open_list(path):
    with open(path, 'r') as fd:
        return parser.parse(fd.read())

if __name__ == '__main__':
    main()
