#include <error.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"

typedef struct
{
    int index;
    int size;
    uint8_t* buffer;
} vm_mini_stream;

typedef union
{
    int64_t integer;
    double  number;
} vm_intnum;

const uint8_t identver[] = "\x89" "Ambrosia Module\r\n\x1a\n\xff\xff";

vm_value read_descriptor(vm_mini_stream *stream, vm_value constants);
vm_value read_string(vm_mini_stream *stream);
vm_value read_bytes(vm_mini_stream *stream);
int read_ushort(vm_mini_stream *stream);
int read_short(vm_mini_stream *stream);
uint32_t read_uint(vm_mini_stream *stream);
int64_t read_integer(vm_mini_stream *stream);
double read_double(vm_mini_stream *stream);
int read_byte(vm_mini_stream *stream);

vm_value open_module(const char* path)
{
    int fd;
    struct stat stat;
    size_t size;
    int i, length;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        error(1, errno, "cannot open module");
    }
    if (fstat(fd, &stat) < 0)
    {
        error(1, errno, "cannot stat module");
    }
    size = stat.st_size;

    uint8_t buffer[size];
    if (read(fd, buffer, size) != size)
    {
        error(1, errno, "cannot read module");
    }

    if (size < sizeof(identver) - 1)
    {
        error(1, 0, "not a module file (too small)");
    }
    for (i = 0; i < sizeof(identver) - 1; i++)
    {
        if (identver[i] != buffer[i])
        {
            error(1, 0, "not a module file %i: %i != %i", i, identver[i], buffer[i]);
        }
    }

    vm_mini_stream _stream = {sizeof(identver) - 1, size, buffer};
    vm_mini_stream *stream = &_stream;

    //vm_value source;
    vm_value constants;
    vm_value baselib;
    vm_value name;
    vm_value value;
    vm_value descriptor;

    read_string(stream); //source    = read_string(stream);
    baselib   = vm_init();
    constants = vm_new_list();

    // globs
    length = read_ushort(stream);
    printf("globs %i\n", length);
    for (i = 0; i < length; i++)
    {
        name = read_string(stream);
        if (0 == vm_dict_getitem(baselib, name, &value))
        {
            error(1, 0, "%s not in baselib\n", vm_unbox_string(name));
        }
        vm_list_append(constants, value);
    }
    // integers
    printf("integers %i\n", length);
    length = read_ushort(stream);
    for (i = 0; i < length; i++)
    {
        value = vm_box_integer(read_integer(stream));
        vm_list_append(constants, value);
    }

    // floats
    length = read_ushort(stream);
    printf("floats %i\n", length);
    for (i = 0; i < length; i++)
    {
        value = vm_box_double(read_double(stream));
        vm_list_append(constants, value);
    }
    // strings
    printf("strings %i\n", length);
    length = read_ushort(stream);
    for (i = 0; i < length; i++)
    {
        value = read_string(stream);
        vm_list_append(constants, value);
    }
    
    descriptor = read_descriptor(stream, constants);
    return descriptor;
}

vm_value read_descriptor(vm_mini_stream *stream, vm_value constants)
{
    int valc, argc, upvalc, upcopyc, size, locs, descs;
    int i;
    uint8_t* program;

    valc = read_ushort(stream);
    argc = read_short(stream);
    upvalc = read_ushort(stream);
    upcopyc = read_ushort(stream);
    printf("valc=%i, argc=%i, upvalc=%i, upcopyc=%i\n", valc, argc, upvalc, upcopyc);
    uint16_t upcopy[upcopyc];
    for (i = 0; i < upcopyc; i++)
    {
        upcopy[i] = read_ushort(stream);
    }
    size = read_ushort(stream);
    program = &stream->buffer[stream->index];
    stream->index += size;
    locs = read_ushort(stream);

    for (i = 0; i < locs; i++)
    {
        read_uint(stream); // skip for now..
    }

    vm_value functions = vm_new_list();

    descs = read_ushort(stream);
    for (i = 0; i < descs; i++)
    {
        vm_list_append(functions, read_descriptor(stream, constants));
    }
    return vm_box_object(vm_new_descriptor(argc, valc, upvalc, upcopyc, size, upcopy, program, functions, constants));
}

vm_value read_string(vm_mini_stream *stream)
{
    int length;
    char* data;

    length = read_ushort(stream);
    data = (char*)&stream->buffer[stream->index];
    stream->index += length;
    return vm_box_string(length, data);
}

int read_ushort(vm_mini_stream *stream)
{
    int value = 0;
    value |= read_byte(stream) << 0;
    value |= read_byte(stream) << 8;
    return value;
}

int read_short(vm_mini_stream *stream)
{
    int value = read_ushort(stream);

    if (value >= 0x8000) { value = value - 0x10000; }

    return value;
}

uint32_t read_uint(vm_mini_stream *stream)
{
    int64_t value = 0;
    int i;

    for (i = 0; i < 4; i++)
    {
        value |= read_byte(stream) << (i*8);
    }
    return value;
}

int64_t read_integer(vm_mini_stream *stream)
{
    int64_t value = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        value |= read_byte(stream) << (i*8);
    }
    return value;
}

double read_double(vm_mini_stream *stream)
{
    vm_intnum intnum;

    intnum.integer = read_integer(stream);
    return intnum.number;
}

int read_byte(vm_mini_stream *stream)
{ 
    if (stream->index >= stream->size) error(1, 0, "overflow during module read");
    return stream->buffer[stream->index++];
}
