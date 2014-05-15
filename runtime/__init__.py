from ctypes import CDLL, c_void_p, c_int, c_double, c_size_t, c_char_p, c_long
import os
import re

directory = os.path.dirname(__file__) or '.'

rti = CDLL(os.path.join(directory, 'runtime.so'))
op_pattern = re.compile("^#define OP_(\w+) +(\d+)$", re.MULTILINE)

optable = {}
with open(os.path.join(directory, 'interpreter.c')) as fd:
    for name, opcode in op_pattern.findall(fd.read()):
        optable[name] = int(opcode)


def abc(name, a=0, b=0, c=0):
    return chr(optable[name.upper()]) + chr(a) + chr(b) + chr(c)

def ad(name, a=0, d=0):
    return chr(optable[name.upper()]) + chr(a) + chr(d >> 8) + chr(d & 255)

def rewrite_d(inst, d=0):
    return inst[0] + inst[1] + chr(d >> 8) + chr(d & 255)

Value = c_void_p

boxString = rti.boxString
boxString.argtypes = [c_size_t, c_char_p]
boxString.restype = Value

isString = rti.isString
isString.argtypes = [c_void_p]
isString.restype  = c_int

unboxString = rti.unboxString
unboxString.argtypes = [c_void_p]
unboxString.restype  = c_char_p

unboxStringLength = rti.unboxStringLength
unboxStringLength.argtypes = [c_void_p]
unboxStringLength.restype  = c_size_t

boxSysFunc = rti.boxSysFunc
boxSysFunc.argtypes = [c_void_p]
boxSysFunc.restype = Value

boxFixnum = rti.boxFixnum
boxFixnum.argtypes = [c_long]
boxFixnum.restype = Value

sysfuncs = {
    'input': boxSysFunc(rti.sysInput),
    'print': boxSysFunc(rti.sysPrint),
}

if __name__=='__main__':
    stringy = boxString(5, "hello")

    print hex(stringy)
    print isString(stringy)
    print unboxStringLength(stringy)
    print repr(unboxString(stringy))
    #print rti.unboxStringLength(stringy)


    program = abc('return', 0, 1)
    var     = (Value*8)()
    g       = 0

    rti.interpret(program, var, g)
