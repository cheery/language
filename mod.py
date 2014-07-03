import json, os

identification = '\x89Ambrosia Module\r\n\x1a\n'

version = '\xff\xff'

instructions = [
    'add', 'and', 'booland', 'boolnot', 'boolor', 'call', 'callvarg', 'closure',
    'div', 'eq', 'except', 'finally', 'floordiv', 'ge', 'getattr', 'getconst',
    'getitem', 'getupval', 'gt', 'jump', 'le', 'loadfalse', 'loadnull', 'loadtrue',
    'lshift', 'lt', 'mod', 'move', 'mul', 'ne', 'neg', 'next', 'not', 'or', 'pos',
    'raise', 'reraise', 'return', 'rshift', 'self', 'setattr', 'setconst', 'setitem',
    'setupval', 'sub', 'tailcall', 'tailcallvarg', 'test', 'testnot', 'xor'
]

class Constants(object):
    def __init__(self, globs, integers, floats, strings):
        self.globs    = globs
        self.integers = integers
        self.floats   = floats
        self.strings  = strings
        self.frozen   = False

    def glob(self, name):
        if name not in self.globs:
            if self.frozen:
                raise TypeError("Object is immutable")
            self.globs.append(name)
        return self.globs.index(name)
    
    def val(self, constant):
        if isinstance(constant, (int, long)):
            tab = self.integers
            off = len(self.globs)
        elif isinstance(constant, float):
            tab = self.floats
            off = len(self.globs) + len(self.integers)
        elif isinstance(constant, (unicode, str)):
            tab = self.strings
            off = len(self.globs) + len(self.integers) + len(self.floats)
        else:
            raise NotImplemented()
        if constant not in tab:
            if self.frozen:
                raise TypeError("Object is immutable")
            tab.append(constant)
        return tab.index(constant) + off

    def __contains__(self, name):
        if name in self.globs:
            return True

    @classmethod
    def empty(cls):
        return cls([], [], [], [])

class Desc(object):
    def __init__(self, flags, valc, argc, nupvalc, upcopy, code, locs, decls):
        self.flags = flags
        self.valc = valc
        self.argc = argc
        self.nupvalc = nupvalc
        self.upcopy = upcopy
        self.code = code
        self.locs = locs
        self.decls = decls

    @classmethod
    def empty(cls):
        return cls(0, 0, 0, 0, [], '', [], [])

bytecode_path = os.path.join(
    os.path.dirname(__file__),
    "build/ambrosia-bytecode.json"
)
with open(bytecode_path) as fd:
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
