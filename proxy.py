import ctypes
import json
import sys
from ctypes import c_size_t, c_char_p, c_void_p, c_ushort, c_int, c_double, POINTER, byref
vm_value = ctypes.c_ulong
c_ushort_p = POINTER(c_ushort)

lib = ctypes.CDLL("build/libambrosia.so")

new_descriptor = lib.vm_new_descriptor
new_descriptor.restype = ctypes.c_void_p
new_descriptor.argtypes = [c_size_t, c_size_t, c_size_t, c_size_t, c_size_t, c_ushort_p, c_char_p, vm_value, vm_value]

new_closure = lib.vm_new_closure
new_closure.restype = c_void_p
new_closure.argtypes = [c_void_p, c_void_p]

new_context = lib.vm_new_context
new_context.restype = c_void_p
new_context.argtypes = [c_void_p]

#context_push = lib.vm_context_push
#context_push.restype = None
#context_push.argtypes = [c_void_p, c_void_p, c_void_p, c_void_p, c_int]

context_bootup = lib.vm_context_bootup
context_bootup.restype = None
context_bootup.argtypes = [c_void_p, c_void_p]

box_object = lib.vm_box_object
box_object.restype = c_void_p
box_object.argtypes = [c_int]

box_double = lib.vm_box_double
box_double.restype = c_void_p
box_double.argtypes = [c_double]

box_cstring = lib.vm_box_cstring
box_cstring.restype = c_void_p
box_cstring.argtypes = [c_char_p]

loop = lib.vm_loop
loop.restype = None
loop.argtypes = [c_void_p]

new_list = lib.vm_new_list
new_list.restype = vm_value
new_list.argtypes = []

list_append = lib.vm_list_append
list_append.restype = None
list_append.argtypes = [vm_value, vm_value]

init = lib.vm_init
init.restype = vm_value
init.argtypes = []

dict_getitem = lib.vm_dict_getitem
dict_getitem.restype = c_int
dict_getitem.argtypes = [vm_value, vm_value, POINTER(vm_value)]

baselib = init()

with open("build/ambrosia-bytecode.json") as fd:
    optable = json.load(fd)

def bcode_abc(opcode, a=0, b=0, c=0):
    return chr(opcode) + chr(a) + chr(b) + chr(c)
def bcode_ad(opcode, a=0, d=0):
    return chr(opcode) + chr(a) + chr(d & 255) + chr(d >> 8)

def bcode(name, *args):
    op = optable[name]
    opcode = op['opcode']
    arity  = len(op['format'])
    if len(args) < arity:
        raise TypeError("{}: expected {} arguments, got {}".format(name, arity, len(args)))
    if arity < 3:
        return bcode_ad(opcode, *args)
    else:
        return bcode_abc(opcode, *args)

def bcode_adjust(bcode, d):
    return bcode[:2] + chr(d & 255) + chr(d >> 8)

def get_global(name):
    item = vm_value()
    if 0 == dict_getitem(baselib, box_cstring(name), byref(item)):
        raise Exception("{} not in globals".format(name))
    return item

def build_descriptor(argc, valc, upvalc, upcopies, bytecode, functions, constants):
    upcopyc = len(upcopies)
    upcopy = (c_ushort*upcopyc)()
    for i, v in enumerate(upcopies):
        upcopy[i] = v

    p_functions = new_list()
    for func in functions:
        list_append(p_functions, func)
    p_constants = new_list()
    for const in constants:
        list_append(p_constants, const)

    return new_descriptor(argc, valc, upvalc, upcopyc, len(bytecode), upcopy, bytecode, p_functions, p_constants)

def run_toplevel(descriptor):
    closure = new_closure(descriptor, None)
    context = new_context(None)
    sys.stderr.write("boot\n")
    context_bootup(context, closure)
    #context_push(context, box_object(0), closure, None, 0)
    sys.stderr.write("trying to evaluate {}, {}, {}\n".format(descriptor, closure, context))
    loop(context)
    sys.stderr.write("success\n")

if __name__ == '__main__':
    code = ''.join([
        bcode('loadnull', 0, 1),
        bcode('setupval', 0, 0),
        bcode('setupval', 0, 1),
        bcode('setupval', 0, 2),
        bcode('loadbool', 0, 1),
        bcode('return', 0),
    ])

    argc = 0
    valc = 1
    upvalc  = 4
    upcopyc = 0
    assert upcopyc == 0

    functions = new_list();
    constants = new_list();

    descr = new_descriptor(argc, valc, upvalc, upcopyc, len(code), None, code, functions, constants)
    closure = new_closure(descr, None)
    ctx = new_context()
    context_bootup(ctx, closure)
    #context_push(ctx, closure, None, 0)

    loop(ctx)

#initRuntime = lib.initRuntime
#initRuntime.restype = Value
#initRuntime.argtypes = []
#
#boxObject = lib.boxObject
#boxObject.restype = Value
#boxObject.argtypes = [ctypes.c_void_p]
#
#boxBoolean = lib.boxBoolean
#boxBoolean.restype = Value
#boxBoolean.argtypes = [ctypes.c_int]
#
#boxFixnum = lib.boxFixnum
#boxFixnum.restype = Value
#boxFixnum.argtypes = [ctypes.c_long]
#
#boxDouble = lib.boxDouble
#boxDouble.restype = Value
#boxDouble.argtypes = [ctypes.c_double]
#
#boxCString = lib.boxCString
#boxCString.restype = Value
#boxCString.argtypes = [ctypes.c_char_p]
#
#unboxString = lib.unboxString
#unboxString.restype = Value
#unboxString.argtypes = [ctypes.c_char_p]
#
#opCall = lib.opCall
#opCall.restype = Value
#opCall.argtypes = [Value, Value, ctypes.c_int, ctypes.POINTER(Value)]
#
#opCallAttr = lib.opCallAttr
#opCallAttr.restype = Value
#opCallAttr.argtypes = [Value, Value, ctypes.c_int, ctypes.POINTER(Value)]
#
#opGetAttr = lib.opGetAttr
#opGetAttr.restype = Value
#opGetAttr.argtypes = [Value, Value]
#
#opSetAttr = lib.opSetAttr
#opSetAttr.restype = Value
#opSetAttr.argtypes = [Value, Value, Value]
#
#opGetItem = lib.opGetItem
#opGetItem.restype = Value
#opGetItem.argtypes = [Value, Value]
#
#opSetItem = lib.opSetItem
#opSetItem.restype = Value
#opSetItem.argtypes = [Value, Value, Value]
#
#def to_value(pyval):
#    if isinstance(pyval, bool):
#        return boxBoolean(pyval)
#    if isinstance(pyval, (str, unicode)):
#        return boxCString(pyval)
#    if isinstance(pyval, float):
#        return boxDouble(pyval)
#    if isinstance(pyval, (int, long)):
#        return boxFixnum(pyval)
#    if isinstance(pyval, Proxy):
#        return pyval._as_parameter_
#    if isinstance(pyval, None):
#        return null._as_parameter_
#    raise Exception("what is this? %r", pyval)
#
#class Proxy(object):
#    __slots__ = ('value', 'this', 'name')
#    def __init__(self, value=None, this=None, name=None):
#        self.value = value 
#        self.this = this 
#        self.name = name 
#
#    @property
#    def _as_parameter_(self):
#        if self.value is None:
#            self.value = opGetAttr(self.this, to_value(self.name))
#        return self.value
#
#    def __call__(self, *args):
#        argc = len(args)
#        argv = (Value*argc)(*map(to_value, args))
#        if self.value is None:
#            return Proxy(opCallAttr(self.this, to_value(self.name), argc, argv))
#        else:
#            return Proxy(opCall(null, self, argc, argv))
#
#    def __getitem__(self, name):
#        return Proxy(opGetItem(self, to_value(name)))
#
#    def __setitem__(self, name, value):
#        opSetItem(self, to_value(name), to_value(value))
#
#    def __getattr__(self, name):
#        return Proxy(None, self, name)
#
#    def __setattr__(self, name, value):
#        try:
#            object.__setattr__(self, name, value)
#        except AttributeError as e:
#            opSetAttr(self, to_value(name), to_value(value))
#
#null = Proxy(boxObject(0))
#namespace = Proxy(initRuntime())
#
#
#namespace["print"]("number: ", 15)
#namespace["print"]("number: ", 1.234)
#
#
#vm_list = namespace["list"]
#closure_descriptor = namespace["closure_descriptor"]
#closure = namespace["closure"]
#
#namespace["print"](closure)
#namespace["print"](closure_descriptor)
#
#descr = closure_descriptor()
#descr.argc = 5
#descr.varc = 12
#descr.upvalc = 8
#namespace["print"]("descr argc: ", descr.argc)
#namespace["print"]("descr varc: ", descr.varc)
#namespace["print"]("descr upvalc: ", descr.upvalc)
#
#lst = vm_list()
#lst.append(1)
#lst.append(2)
#lst.append(3)
#
#namespace["print"]("list: ", lst)
