#include "snakelisp.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static CONTINUATION(pick)
{
    value_t cond = ARG(2);
    if (isFalse(cond) || isNull(cond))
    {
        call(ARG(4), ARG(1));
    }
    else
    {
        call(ARG(3), ARG(1));
    }
}

static CONTINUATION(new_arraybuffer)
{
    call(ARG(1), boxArrayBuffer(newArrayBuffer(ARG_INTEGER(2))));
}

static CONTINUATION(file_open)
{
    int fd;
    string_t *path = ARG_STRING(2);
    printf("achievement: file open %s\n", path->data);
    fd = open(path->data, O_RDONLY);
    call(ARG(1), boxInteger(fd));
}

static CONTINUATION(file_close)
{
    close(ARG_INTEGER(2));
    call(ARG(1), boxNull());
}

static CONTINUATION(file_read)
{
    printf("achievement: file read\n");
    int fd = ARG_INTEGER(2);
    arraybuffer_t *buffer = ARG_ARRAYBUFFER(3);
    size_t count = buffer->length;
    if (argc > 4) count = ARG_INTEGER(4);

    call(ARG(1), boxInteger(read(fd, buffer->data, count)));
}

CONTINUATION(file_write)
{
    printf("achievement: file write\n");
    int fd = ARG_INTEGER(2);
    size_t count;
    void  *data;
    if (isArrayBuffer(ARG(3)))
    {
        arraybuffer_t *arraybuffer = unboxArrayBuffer(ARG(3));
        data  = arraybuffer->data;
        count = arraybuffer->length;
    }
    else if (isString(ARG(3)))
    {
        string_t *string = unboxString(ARG(3));
        data  = string->data;
        count = string->length;
    }
    else
    {
        ARG_ERROR(3, "arraybuffer or string");
    }
    if (argc > 4) count = ARG_INTEGER(4);
    call(ARG(1), boxInteger(write(fd, data, count)));
}

//CONTINUATION(c_length)
//{
//    value_t cont = c_get_argument(args, 1);
//    value_t obj  = c_get_argument(args, 2);
//    size_t length;
//    switch (obj.type)
//    {
//        case TYPE_STRING:
//            length = obj.data.string->length;
//            break;
//        case TYPE_ARRAYBUFFER:
//            length = obj.data.arraybuffer->length;
//            break;
//        default:
//            assert(obj.type == TYPE_ARRAYBUFFER);
//    }
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_integer(length));
//    c_call_end();
//}
//
//CONTINUATION(c_idx)
//{
//    value_t cont = c_get_argument(args, 1);
//    value_t obj  = c_get_argument(args, 2);
//    size_t  idx  = unbox_integer(c_get_argument(args, 3));
//    value_t retval;
//    switch (obj.type)
//    {
//        case TYPE_STRING:
//            assert(idx < obj.data.string->length);
//            retval = c_const_string2(&obj.data.string->data[idx], 1);
//            break;
//        case TYPE_ARRAYBUFFER:
//            assert(idx < obj.data.arraybuffer->length);
//            retval = c_const_integer(obj.data.arraybuffer->data[idx]);
//            break;
//        default:
//            assert(obj.type == TYPE_ARRAYBUFFER);
//    }
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, retval);
//    c_call_end();
//}
//
//CONTINUATION(c_set_idx)
//{
//    value_t cont = c_get_argument(args, 1);
//    value_t obj  = c_get_argument(args, 2);
//    size_t  idx  = unbox_integer(c_get_argument(args, 3));
//    value_t val  = c_get_argument(args, 4);
//    switch (obj.type)
//    {
//        case TYPE_ARRAYBUFFER:
//            assert(idx < obj.data.arraybuffer->length);
//            obj.data.arraybuffer->data[idx] = unbox_integer(val);
//            break;
//        default:
//            assert(obj.type == TYPE_ARRAYBUFFER);
//    }
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, val);
//    c_call_end();
//}
//
//#define ARG(name, k) value_t name = c_get_argument(args, k);
//
//CONTINUATION(c_eq)
//{
//    ARG(cont, 1);
//    ARG(lhs,  2);
//    ARG(rhs,  3);
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_boolean(lhs.type==rhs.type && lhs.data.integer == rhs.data.integer));
//    c_call_end();
//}
//
//CONTINUATION(c_ne)
//{
//    ARG(cont, 1);
//    ARG(lhs,  2);
//    ARG(rhs,  3);
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_boolean(lhs.type!=rhs.type || lhs.data.integer != rhs.data.integer));
//    c_call_end();
//}
//
//
//void c_boot(code_t entry)
//{
//    closure_t closure = {entry,  0};
//    closure_t cont    = {c_quit, 0}; 
//
//    closure_t pick        = {c_pick, 0};
//    closure_t arraybuffer = {c_arraybuffer, 0};
//    closure_t file_open   = {c_file_open, 0};
//    closure_t file_close  = {c_file_close, 0};
//    closure_t file_read   = {c_file_read, 0};
//    closure_t file_write  = {c_file_write, 0};
//    closure_t length      = {c_length, 0};
//    closure_t idx         = {c_idx, 0};
//    closure_t set_idx     = {c_set_idx, 0};
//
//    cl_pick         = c_const_closure(&pick);
//    cl_arraybuffer  = c_const_closure(&arraybuffer);
//    cl_file_open    = c_const_closure(&file_open);
//    cl_file_close   = c_const_closure(&file_close);
//    cl_file_read    = c_const_closure(&file_read);
//    cl_file_write   = c_const_closure(&file_write);
//    cl_length       = c_const_closure(&length);
//    cl_idx          = c_const_closure(&idx);
//    cl_set_idx      = c_const_closure(&set_idx);
//    v_stdin  = c_const_integer(0);
//    v_stdout = c_const_integer(1);
//    v_stderr = c_const_integer(2);
//
//    printf("achievement: bootup\n");
//    c_call_begin(2);
//    c_call_argument(0, c_const_closure(&closure));
//    c_call_argument(1, c_const_closure(&cont));
//    c_call_end();
//}
value_t
    v_pick,
    v_arraybuffer,
    v_file_read,
    v_file_write,
    v_file_open,
    v_file_close,
    v_stdin,
    v_stdout,
    v_stderr;


static CONTINUATION(quit)
{
    printf("achievement: exit through the door\n");
    exit(0);
}

void snakeBoot(value_t entry)
{
    v_pick = spawnClosure(pick);
    v_arraybuffer = spawnClosure(new_arraybuffer);

    v_file_read  = spawnClosure(file_read);
    v_file_write = spawnClosure(file_write);
    v_file_open  = spawnClosure(file_open);
    v_file_close = spawnClosure(file_close);
    v_stdin  = boxInteger(0);
    v_stdout = boxInteger(1);
    v_stderr = boxInteger(2);
    call(entry, spawnClosure(quit));
}
