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

static CONTINUATION(new_array)
{
    call(ARG(1), boxArray(newArray(ARG_INTEGER(2))));
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

static CONTINUATION(file_write)
{
    printf("achievement: file write\n");
    int fd = ARG_INTEGER(2);
    size_t count;
    void  *data;
    if (isArrayBuffer(ARG(3)))
    {
        arraybuffer_t *arraybuffer = ARG_ARRAYBUFFER(3);
        data  = arraybuffer->data;
        count = arraybuffer->length;
    }
    else if (isString(ARG(3)))
    {
        string_t *string = ARG_STRING(3);
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

static CONTINUATION(catenate)
{
    size_t i = 2;
    size_t length = 1;
    size_t offset = 0;
    switch(ARG(2).type)
    {
        case TYPE_STRING:
            {
                for(i = 2; i < argc; i++)
                {
                    length += ARG_STRING(i)->length;
                }
                string_t *result = newString(length, NULL);
                string_t *current;
                for(i = 2; i < argc; i++)
                {
                    current = unboxString(ARG(i));
                    memcpy(&result->data[offset], current->data, current->byteLength);
                    offset += current->length;
                }
                call(ARG(1), boxString(result));
            }
            break;
        default:
            ARG_ERROR(i, "string");
    }
}

static CONTINUATION(length_of)
{
    size_t length;
    switch(ARG(2).type)
    {
        case TYPE_STRING:
            length = ARG_STRING(2)->length;
            break;
        case TYPE_ARRAYBUFFER:
            length = ARG_ARRAYBUFFER(2)->length;
            break;
        default:
            ARG_ERROR(2, "arraybuffer or string");
    }
    call(ARG(1), boxInteger(length));
}

static CONTINUATION(load_idx)
{
    size_t index = ARG_INTEGER(3);
    switch(ARG(2).type)
    {
        case TYPE_STRING:
            {
                string_t *string = ARG_STRING(2);
                if (index >= string->length) assert(1==2);
                call(ARG(1), boxString(newString(1, &string->data[index])));
            }
            break;
        case TYPE_ARRAYBUFFER:
            {
                arraybuffer_t *arraybuffer = ARG_ARRAYBUFFER(2);
                if (index >= arraybuffer->length) assert(1==2);
                call(ARG(1), boxInteger(arraybuffer->data[index]));
            }
            break;
        case TYPE_ARRAY:
            {
                array_t *array = ARG_ARRAY(2);
                if (index >= array->length) assert(1==2);
                call(ARG(1), array->val[index]);
            }
            break;
        default:
            ARG_ERROR(2, "array, arraybuffer or string");
    }
}

static CONTINUATION(store_idx)
{
    size_t index = ARG_INTEGER(3);
    switch(ARG(2).type)
    {
        case TYPE_ARRAY:
            {
                array_t *array = ARG_ARRAY(2);
                if (index >= array->length) assert(1==2);
                array->val[index] = ARG(4);
            }
            break;
        case TYPE_ARRAYBUFFER:
            {
                arraybuffer_t *arraybuffer = ARG_ARRAYBUFFER(2);
                if (index >= arraybuffer->length) assert(1==2);
                arraybuffer->data[index] = ARG_INTEGER(4);
            }
            break;
        default:
            ARG_ERROR(2, "array or arraybuffer");
    }
    call(ARG(1), ARG(4));
}

static CONTINUATION(is_closure)
{
    call(ARG(1), boxBoolean(isClosure(ARG(2))));
}

static CONTINUATION(is_null)
{
    call(ARG(1), boxBoolean(isNull(ARG(2))));
}

static CONTINUATION(is_true)
{
    call(ARG(1), boxBoolean(isTrue(ARG(2))));
}

static CONTINUATION(is_false)
{
    call(ARG(1), boxBoolean(isFalse(ARG(2))));
}

static CONTINUATION(is_boolean)
{
    call(ARG(1), boxBoolean(isBoolean(ARG(2))));
}

static CONTINUATION(is_integer)
{
    call(ARG(1), boxBoolean(isInteger(ARG(2))));
}

static CONTINUATION(is_double)
{
    call(ARG(1), boxBoolean(isDouble(ARG(2))));
}

static CONTINUATION(is_array)
{
    call(ARG(1), boxBoolean(isArray(ARG(2))));
}

static CONTINUATION(is_arraybuffer)
{
    call(ARG(1), boxBoolean(isArrayBuffer(ARG(2))));
}

static CONTINUATION(is_string)
{
    call(ARG(1), boxBoolean(isString(ARG(2))));
}
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
    v_array,
    v_arraybuffer,
    v_file_read,
    v_file_write,
    v_file_open,
    v_file_close,
    v_stdin,
    v_stdout,
    v_stderr,
    v_cat,
    v_length,
    v_load_idx,
    v_store_idx,
    v_is_closure,
    v_is_null,
    v_is_true,
    v_is_false,
    v_is_boolean,
    v_is_integer,
    v_is_double,
    v_is_array,
    v_is_arraybuffer,
    v_is_string;

static CONTINUATION(quit)
{
    printf("achievement: exit through the door\n");
    exit(0);
}

void snakeBoot(value_t entry)
{
    v_pick = spawnClosure(pick);
    v_array       = spawnClosure(new_array);
    v_arraybuffer = spawnClosure(new_arraybuffer);

    v_file_read  = spawnClosure(file_read);
    v_file_write = spawnClosure(file_write);
    v_file_open  = spawnClosure(file_open);
    v_file_close = spawnClosure(file_close);
    v_stdin  = boxInteger(0);
    v_stdout = boxInteger(1);
    v_stderr = boxInteger(2);

    v_cat       = spawnClosure(catenate);
    v_length    = spawnClosure(length_of);
    v_load_idx  = spawnClosure(load_idx);
    v_store_idx = spawnClosure(store_idx);

    v_is_closure = spawnClosure(is_closure);
    v_is_null = spawnClosure(is_null);
    v_is_true = spawnClosure(is_true);
    v_is_false = spawnClosure(is_false);
    v_is_boolean = spawnClosure(is_boolean);
    v_is_integer = spawnClosure(is_integer);
    v_is_double = spawnClosure(is_double);
    v_is_array = spawnClosure(is_array);
    v_is_arraybuffer = spawnClosure(is_arraybuffer);
    v_is_string = spawnClosure(is_string);

    call(entry, spawnClosure(quit));
}
