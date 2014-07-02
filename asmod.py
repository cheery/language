import struct
import mod

def writeout(path, source, const, decl):
    with open(path, 'wb') as fd:
        fd.write(mod.identification)
        fd.write(mod.version)
#        write_string(fd, source)
        write_list(fd, write_string, const.globs)
        write_list(fd, write_integer, const.integers)
        write_list(fd, write_double, const.floats)
        write_list(fd, write_string, const.strings)
        write_desc(fd, decl)

def write_desc(fd, func):
    write_ushort(fd, func.flags)
    write_ushort(fd, func.valc)
    write_short(fd, func.argc)
    write_ushort(fd, func.nupvalc)
    write_list(fd, write_ushort, func.upcopy)
    write_bytes(fd, func.code)
    write_list(fd, write_uint, func.locs)
    write_list(fd, write_desc, func.decls)

def write_list(fd, encoder, values):
    write_ushort(fd, len(values))
    for value in values:
        encoder(fd, value)

def write_string(fd, string):
    string = string.encode('utf-8')
    write_bytes(fd, string)

def write_bytes(fd, data):
    write_ushort(fd, len(data))
    fd.write(data)

def write_ushort(fd, value):
    fd.write(struct.pack('<H', value))

def write_short(fd, value):
    fd.write(struct.pack('<h', value))

def write_uint(fd, value):
    fd.write(struct.pack('<I', value))

def write_integer(fd, value):
    fd.write(struct.pack('<q', value))

def write_double(fd, value):
    fd.write(struct.pack('<d', value))

if __name__=='__main__':
    source = "<less offensive>"
    const  = mod.Constants.empty()
    decl   = mod.Decl.empty()
    writeout('demo.o', source, const, decl)
