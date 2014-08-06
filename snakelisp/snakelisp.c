#include "snakelisp.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CONTINUATION(x) static void x(closure_t *clos, argv_t *args)


CONTINUATION(c_quit)
{
    printf("achievement: quitter.\n");
    exit(0);
}

CONTINUATION(c_pick)
{
    value_t cont = c_get_argument(args, 1);
    value_t cond = c_get_argument(args, 2);
    value_t yes = c_get_argument(args, 3);
    value_t no  = c_get_argument(args, 4);

    c_call_begin(2);
    if (cond.type == TYPE_NULL || (cond.type == TYPE_BOOLEAN && cond.data.integer == 0))
    {
        c_call_argument(0, no);
    }
    else
    {
        c_call_argument(0, yes);
    }
    c_call_argument(1, cont);
    c_call_end();
}

CONTINUATION(c_arraybuffer)
{
    value_t cont = c_get_argument(args, 1);
    value_t size = c_get_argument(args, 2);
    assert (TYPE_INTEGER == size.type);
    value_t a = {TYPE_ARRAYBUFFER};
    a.data.arraybuffer = alloca(sizeof(arraybuffer_t) + sizeof(uint8_t)*size.data.integer);
    a.data.arraybuffer->length = size.data.integer;
    memset(a.data.arraybuffer->data, 0, a.data.arraybuffer->length);
    c_call_begin(2);
    c_call_argument(0, cont);
    c_call_argument(1, a);
    c_call_end();
}

CONTINUATION(c_file_open)
{
    value_t cont = c_get_argument(args, 1);
    value_t path = c_get_argument(args, 2);
    int fd;
    assert (TYPE_STRING == path.type);
    printf("achievement: file open %s\n", path.data.string->data);
    fd = open(path.data.string->data, O_RDONLY);
    c_call_begin(2);
    c_call_argument(0, cont);
    if (fd < 0)
    {
        c_call_argument(1, c_const_null());
    }
    else
    {
        c_call_argument(1, c_const_integer(fd));
    }
    c_call_end();
}

CONTINUATION(c_file_close)
{
    value_t cont   = c_get_argument(args, 1);
    value_t fileno = c_get_argument(args, 2);
    printf("achievement: file close\n");
    assert (fileno.type == TYPE_INTEGER);
    close(fileno.data.integer);
    c_call_begin(2);
    c_call_argument(0, cont);
    c_call_argument(1, c_const_null());
    c_call_end();
}

CONTINUATION(c_file_read)
{
    printf("achievement: file read\n");
    value_t cont   = c_get_argument(args, 1);
    value_t fileno = c_get_argument(args, 2);
    value_t data   = c_get_argument(args, 3);
    value_t coun   = c_get_argument(args, 4);
    assert (fileno.type == TYPE_INTEGER);
    assert (data.type   == TYPE_ARRAYBUFFER);
    size_t size = data.data.arraybuffer->length;
    if (coun.type != TYPE_NULL)
    {
        assert(coun.type == TYPE_INTEGER);
        size = coun.data.integer;
    }
    c_call_begin(2);
    c_call_argument(0, cont);
    c_call_argument(1, c_const_integer(read(fileno.data.integer, data.data.arraybuffer->data, size)));
    c_call_end();
}

CONTINUATION(c_file_write)
{
    printf("achievement: file write\n");
    value_t cont = c_get_argument(args, 1);
    value_t fd   = c_get_argument(args, 2);
    value_t data = c_get_argument(args, 3);
    value_t coun = c_get_argument(args, 4);
    size_t size;
    void  *buff;
    if (data.type == TYPE_STRING)
    {
        size = data.data.string->length;
        buff = data.data.string->data;
    }
    else
    {
        assert (data.type == TYPE_ARRAYBUFFER);
        size = data.data.arraybuffer->length;
        buff = data.data.arraybuffer->data;
    }
    assert (fd.type == TYPE_INTEGER);
    if (coun.type != TYPE_NULL)
    {
        assert(coun.type == TYPE_INTEGER);
        size = coun.data.integer;
    }

    c_call_begin(2);
    c_call_argument(0, cont);
    c_call_argument(1, c_const_integer(write(fd.data.integer, buff, size)));
    c_call_end();
}

void c_boot(code_t entry)
{
    closure_t closure = {entry,  0};
    closure_t cont    = {c_quit, 0}; 

    closure_t pick        = {c_pick, 0};
    closure_t arraybuffer = {c_arraybuffer, 0};
    closure_t file_open   = {c_file_open, 0};
    closure_t file_close  = {c_file_close, 0};
    closure_t file_read   = {c_file_read, 0};
    closure_t file_write  = {c_file_write, 0};

    cl_pick         = c_const_closure(&pick);
    cl_arraybuffer  = c_const_closure(&arraybuffer);
    cl_file_open    = c_const_closure(&file_open);
    cl_file_close   = c_const_closure(&file_close);
    cl_file_read    = c_const_closure(&file_read);
    cl_file_write   = c_const_closure(&file_write);
    v_stdin  = c_const_integer(0);
    v_stdout = c_const_integer(1);
    v_stderr = c_const_integer(2);

    printf("achievement: bootup\n");
    c_call_begin(2);
    c_call_argument(0, c_const_closure(&closure));
    c_call_argument(1, c_const_closure(&cont));
    c_call_end();
}
