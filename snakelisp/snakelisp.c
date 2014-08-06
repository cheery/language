#include "snakelisp.h"
#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//
//#define CONTINUATION(x) static void x(closure_t *clos, argv_t *args)
//
//static inline long unbox_integer(value_t value)
//{
//    assert(value.type == TYPE_INTEGER);
//    return value.data.integer;
//}
//
//CONTINUATION(c_quit)
//{
//    printf("achievement: quitter.\n");
//    exit(0);
//}
//
//CONTINUATION(c_pick)
//{
//    value_t cont = c_get_argument(args, 1);
//    value_t cond = c_get_argument(args, 2);
//    value_t yes = c_get_argument(args, 3);
//    value_t no  = c_get_argument(args, 4);
//
//    c_call_begin(2);
//    if (cond.type == TYPE_NULL || (cond.type == TYPE_BOOLEAN && cond.data.integer == 0))
//    {
//        c_call_argument(0, no);
//    }
//    else
//    {
//        c_call_argument(0, yes);
//    }
//    c_call_argument(1, cont);
//    c_call_end();
//}
//
//CONTINUATION(c_arraybuffer)
//{
//    value_t cont = c_get_argument(args, 1);
//    size_t  size = unbox_integer(c_get_argument(args, 2));
//    value_t a = {TYPE_ARRAYBUFFER};
//    a.data.arraybuffer = alloca(sizeof(arraybuffer_t) + sizeof(uint8_t)*size);
//    a.data.arraybuffer->length = size;
//    memset(a.data.arraybuffer->data, 0, a.data.arraybuffer->length);
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, a);
//    c_call_end();
//}
//
//CONTINUATION(c_file_open)
//{
//    value_t cont = c_get_argument(args, 1);
//    value_t path = c_get_argument(args, 2);
//    int fd;
//    assert (TYPE_STRING == path.type);
//    printf("achievement: file open %s\n", path.data.string->data);
//    fd = open(path.data.string->data, O_RDONLY);
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    if (fd < 0)
//    {
//        c_call_argument(1, c_const_null());
//    }
//    else
//    {
//        c_call_argument(1, c_const_integer(fd));
//    }
//    c_call_end();
//}
//
//CONTINUATION(c_file_close)
//{
//    value_t cont   = c_get_argument(args, 1);
//    value_t fileno = c_get_argument(args, 2);
//    printf("achievement: file close\n");
//    assert (fileno.type == TYPE_INTEGER);
//    close(fileno.data.integer);
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_null());
//    c_call_end();
//}
//
//CONTINUATION(c_file_read)
//{
//    printf("achievement: file read\n");
//    value_t cont   = c_get_argument(args, 1);
//    value_t fileno = c_get_argument(args, 2);
//    value_t data   = c_get_argument(args, 3);
//    value_t coun   = c_get_argument(args, 4);
//    assert (fileno.type == TYPE_INTEGER);
//    assert (data.type   == TYPE_ARRAYBUFFER);
//    size_t size = data.data.arraybuffer->length;
//    if (coun.type != TYPE_NULL)
//    {
//        assert(coun.type == TYPE_INTEGER);
//        size = coun.data.integer;
//    }
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_integer(read(fileno.data.integer, data.data.arraybuffer->data, size)));
//    c_call_end();
//}
//
//CONTINUATION(c_file_write)
//{
//    printf("achievement: file write\n");
//    value_t cont = c_get_argument(args, 1);
//    value_t fd   = c_get_argument(args, 2);
//    value_t data = c_get_argument(args, 3);
//    value_t coun = c_get_argument(args, 4);
//    size_t size;
//    void  *buff;
//    if (data.type == TYPE_STRING)
//    {
//        size = data.data.string->length;
//        buff = data.data.string->data;
//    }
//    else
//    {
//        assert (data.type == TYPE_ARRAYBUFFER);
//        size = data.data.arraybuffer->length;
//        buff = data.data.arraybuffer->data;
//    }
//    assert (fd.type == TYPE_INTEGER);
//    if (coun.type != TYPE_NULL)
//    {
//        assert(coun.type == TYPE_INTEGER);
//        size = coun.data.integer;
//    }
//
//    c_call_begin(2);
//    c_call_argument(0, cont);
//    c_call_argument(1, c_const_integer(write(fd.data.integer, buff, size)));
//    c_call_end();
//}
//
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
    v_stdin  = boxInteger(0);
    v_stdout = boxInteger(1);
    v_stderr = boxInteger(2);
    call(entry, spawnClosure(quit));
}
