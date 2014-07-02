import sys
import struct
import mod

def main():
    path = sys.argv[1]
    with open(path, 'rb') as fd:
        if not check(fd, mod.identification):
            raise TypeError("this is not a module file")
        if not check(fd, mod.version):
            raise TypeError("unknown/old version")
#        source = read_string(fd)
        globs    = read_list(fd, read_string)
        integers = read_list(fd, read_integer)
        floats   = read_list(fd, read_double)
        strings  = read_list(fd, read_string)
        #print "source={!r}".format(source)
        print "globs   ={}".format(globs)
        print "integers={}".format(integers)
        print "floats  ={}".format(floats)
        print "strings ={}".format(strings)
        decl = read_decl(fd)

        print "valc={}".format(decl.valc)
        print "argc={}".format(decl.argc)
        print "upvalc={}".format(decl.upvalc)
        print "upcopy={}".format(decl.upcopy)
        print "code={!r}".format(decl.code)
        print "locs={!r}".format(decl.locs)
        print "decls={!r}".format(decl.decls)

def read_decl(fd):
    flags = read_ushort(fd)
    valc = read_ushort(fd)
    argc = read_short(fd)
    nupvalc = read_ushort(fd)
    upcopy = read_list(fd, read_ushort)
    code = read_bytes(fd)
    locs = read_list(fd, read_uint)
    decls = read_list(fd, read_decl)
    return mod.Desc(flags, valc, argc, nupvalc, upcopy, code, locs, decls)

def read_list(fd, decoder):
    length = read_ushort(fd)
    lst = []
    for i in range(length):
        lst.append(decoder(fd))
    return lst

def read_string(fd):
    return read_bytes(fd).decode('utf-8')

def read_bytes(fd):
    length = read_ushort(fd)
    return fd.read(length)

def read_ushort(fd):
    return struct.unpack('<H', fd.read(2))[0]

def read_short(fd):
    return struct.unpack('<h', fd.read(2))[0]

def read_uint(fd, value):
    return struct.unpack('<I', fd.read(4))[0]

def read_integer(fd):
    return struct.unpack('<q', fd.read(8))[0]

def read_double(fd):
    return struct.unpack('<d', fd.read(8))[0]

def check(fd, string):
    return string == fd.read(len(string))

if __name__=='__main__':
    main()
