#include "vm.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <stdio.h>

typedef struct
{
    size_t index;
    size_t size;
    uint8_t* buffer;
} mini_stream;

const uint8_t identver[] = "\x89" "Ambrosia Module\r\n\x1a\n\xff\xff";

vm_descriptor* read_descriptor(vm_context *ctx, mini_stream *stream, vm_list *constants);
vm_string* read_string(vm_context *ctx, mini_stream *stream);
int      read_ushort(mini_stream *stream);
int      read_short(mini_stream *stream);
uint32_t read_uint(mini_stream *stream);
int64_t  read_integer(mini_stream *stream);
double   read_double(mini_stream *stream);
int      read_byte(mini_stream *stream);

vm_descriptor* vm_module_load(vm_context *ctx, vm_path *path, vm_dict *baselib, vm_dict *names)
{
    int fd;
    struct stat stat;
    size_t i, length;
    mini_stream _stream;
    mini_stream *stream = &_stream;
    vm_list *constants;
    vm_val name, value;

    fd = open(vm_path_cpath(ctx, path)->data, O_RDONLY);
    if (fd < 0)
    {
        error(1, errno, "cannot open module");
    }
    if (fstat(fd, &stat) < 0)
    {
        error(1, errno, "cannot stat module");
    }
    uint8_t buffer[stat.st_size];
    stream->size   = stat.st_size;
    stream->buffer = buffer;
    printf("size = %li\n", stat.st_size);
    if (read(fd, stream->buffer, stream->size) != stream->size)
    {
        error(1, errno, "cannot read module");
    }
    if (stream->size < sizeof(identver) - 1)
    {
        error(1, 0, "not a module file (too small)");
    }
    for (i = 0; i < sizeof(identver) - 1; i++)
    {
        if (identver[i] != buffer[i])
        {
            error(1, 0, "not a module file %li: %i != %i", i, identver[i], buffer[i]);
        }
    }
    stream->index = sizeof(identver) - 1;
    //read_string(ctx, stream); // source =
    constants = vm_new_list(ctx, interface_stub);

    length = read_ushort(stream);
    printf("globs count: %li\n", length);
    // globs
    for (i = 0; i < length; i++)
    {
        name = vm_box(ctx, read_string(ctx, stream));
        if (0 == vm_dict_getitem(ctx, baselib, name, &value))
        if (0 == vm_dict_getitem(ctx, names, name, &value))
        {
            error(1, 0, "%s not in scope\n", ((vm_string*)vm_unbox(ctx, name, vm_t_string))->data);
        }
        vm_list_append(ctx, constants, value);
    }
    // integers
    length = read_ushort(stream);
    printf("integers count: %li\n", length);
    for (i = 0; i < length; i++)
    {
        value = vm_box_integer(ctx, read_integer(stream));
        vm_list_append(ctx, constants, value);
    }

    // floats
    length = read_ushort(stream);
    printf("floats count: %li\n", length);
    for (i = 0; i < length; i++)
    {
        value = vm_box_double(ctx, read_double(stream));
        vm_list_append(ctx, constants, value);
    }
    // strings
    printf("strings count: %li\n", length);
    length = read_ushort(stream);
    for (i = 0; i < length; i++)
    {
        value = vm_box(ctx, read_string(ctx, stream));
        vm_list_append(ctx, constants, value);
    }
    return read_descriptor(ctx, stream, constants);
}

vm_descriptor* read_descriptor(vm_context *ctx, mini_stream *stream, vm_list *constants)
{
    int flags, valc, argc, nupvalc, upcopyc, size, locs, descs;
    int i;
    vm_arraybuffer *upcopy;
    vm_arraybuffer *program;
    vm_list        *functions;

    flags = read_ushort(stream);
    valc = read_ushort(stream);
    argc = read_ushort(stream);
    nupvalc = read_ushort(stream);

    upcopyc = read_ushort(stream);
    upcopy = vm_new_arraybuffer(ctx, upcopyc, NULL);
    uint16_t *n = (uint16_t*)upcopy->data;
    for (i = 0; i < upcopyc; i++)
    {
        n[i] = read_ushort(stream);
    }

    printf("valc=%i, argc=%i, nupvalc=%i, upcopyc=%i\n", valc, argc, nupvalc, upcopyc);
    size = read_ushort(stream); 
    program = vm_new_arraybuffer(ctx, size, &stream->buffer[stream->index]);
    stream->index += size;

    locs = read_ushort(stream);
    for (i = 0; i < locs; i++)
    {
        read_uint(stream); // skip for now..
    }

    functions = vm_new_list(ctx, interface_stub);

    descs = read_ushort(stream);
    for (i = 0; i < descs; i++)
    {
        vm_list_append(ctx, functions, vm_box(ctx, read_descriptor(ctx, stream, constants)));
    }
    return vm_new_descriptor(ctx, flags, argc, valc, nupvalc, upcopy, program, functions, constants);
}

vm_string* read_string(vm_context *ctx, mini_stream *stream)
{
    int length;
    char* data;

    length = read_ushort(stream);
    data = (char*)&stream->buffer[stream->index];
    stream->index += length;
    //printf("length: %i\n", length);
    return vm_new_string(ctx, length, data);
}

int read_ushort(mini_stream *stream)
{
    int value = 0;
    value |= read_byte(stream) << 0;
    value |= read_byte(stream) << 8;
    return value;
}

int read_short(mini_stream *stream)
{
    int value = read_ushort(stream);

    if (value >= 0x8000) { value = value - 0x10000; }

    return value;
}

uint32_t read_uint(mini_stream *stream)
{
    int64_t value = 0;
    int i;

    for (i = 0; i < 4; i++)
    {
        value |= read_byte(stream) << (i*8);
    }
    return value;
}

int64_t read_integer(mini_stream *stream)
{
    int64_t value = 0;
    int i;

    for (i = 0; i < 8; i++)
    {
        value |= read_byte(stream) << (i*8);
    }
    return value;
}

double read_double(mini_stream *stream)
{
    union { int64_t integer; double  number; } pun;
    pun.integer = read_integer(stream);
    return pun.number;
}

int read_byte(mini_stream *stream)
{ 
    if (stream->index >= stream->size) error(1, 0, "overflow during module read");
    //printf("read single byte %i\n", stream->buffer[stream->index]);
    return stream->buffer[stream->index++];
}
