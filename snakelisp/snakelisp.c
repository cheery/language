#include "snakelisp.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <setjmp.h>
#include <sys/time.h>
#include <sys/resource.h>

static CONTINUATION(call_cc_continue)
{
    call(*SLOT(0), ARG(2));
}

static CONTINUATION(call_cc)
{
    value_t cont = ARG(1);
    value_t func = spawnClosure(call_cc_continue, &cont);
    call(ARG(2), func);
}

static CONTINUATION(pick)
{
    if (unboxBoolean(ARG(2)))
    {
        call(ARG(3), ARG(1));
    }
    else
    {
        call(ARG(4), ARG(1));
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
    int fd = ARG_INTEGER(2);
    arraybuffer_t *buffer = ARG_ARRAYBUFFER(3);
    size_t count = buffer->length;
    if (argc > 4) count = ARG_INTEGER(4);
    count = read(fd, buffer->data, count);
    call(ARG(1), boxInteger(count));
}

static CONTINUATION(file_write)
{
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
        case TYPE_ARRAY:
            length = ARG_ARRAY(2)->length;
            break;
        default:
            ARG_ERROR(2, "array, arraybuffer or string");
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

#define COMPARATOR(op) \
{ \
    long truth = false; \
    value_t a = ARG(2); \
    value_t b = ARG(3); \
 \
    if (coercesToInteger(a) && coercesToInteger(b)) \
    { \
        truth = ARG_INTEGER(2) op ARG_INTEGER(3); \
    } \
    else if (coercesToDouble(a) && coercesToDouble(b)) \
    { \
        truth = ARG_DOUBLE(2) op ARG_DOUBLE(3); \
    } \
    else if (isString(a) && isString(b)) \
    { \
        string_t *astr = ARG_STRING(2); \
        string_t *bstr = ARG_STRING(3); \
        truth = (strcmp(astr->data, bstr->data) op 0); \
    } \
    else \
    { \
        assert(false); /* not implemented for this value pair. */ \
    } \
    call(ARG(1), boxBoolean(truth)); \
}

static CONTINUATION(op_lt) COMPARATOR(<)
static CONTINUATION(op_le) COMPARATOR(<=)
static CONTINUATION(op_gt) COMPARATOR(>)
static CONTINUATION(op_ge) COMPARATOR(>=)

static CONTINUATION(op_eq)
{
    long truth = false;
    value_t a = ARG(2);
    value_t b = ARG(3);

    if (coercesToInteger(a) && coercesToInteger(b))
    {
        truth = ARG_INTEGER(2) == ARG_INTEGER(3);
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        truth = ARG_DOUBLE(2) == ARG_DOUBLE(3);
    }
    else if (isString(a) && isString(b))
    {
        string_t *astr = ARG_STRING(2);
        string_t *bstr = ARG_STRING(3);
        truth = (strcmp(astr->data, bstr->data) == 0);
    }
    else
    {
        truth = (a.type == b.type) && (a.a.address == b.a.address);
    }
    call(ARG(1), boxBoolean(truth));
}

static CONTINUATION(op_ne)
{
    long truth = false;
    value_t a = ARG(2);
    value_t b = ARG(3);

    if (coercesToInteger(a) && coercesToInteger(b))
    {
        truth = ARG_INTEGER(2) != ARG_INTEGER(3);
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        truth = ARG_DOUBLE(2) != ARG_DOUBLE(3);
    }
    else if (isString(a) && isString(b))
    {
        string_t *astr = ARG_STRING(2);
        string_t *bstr = ARG_STRING(3);
        truth = (strcmp(astr->data, bstr->data) != 0);
    }
    else
    {
        truth = (a.type != b.type) || (a.a.address != b.a.address);
    }
    call(ARG(1), boxBoolean(truth));
}




static CONTINUATION(op_add)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) + ARG_INTEGER(3)));
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        call(ARG(1), boxDouble(ARG_DOUBLE(2) + ARG_DOUBLE(3)));
    }
    else
    {
        ARG_ERROR(2, "integer or double");
    }
}

static CONTINUATION(op_sub)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) - ARG_INTEGER(3)));
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        call(ARG(1), boxDouble(ARG_DOUBLE(2) - ARG_DOUBLE(3)));
    }
    else
    {
        ARG_ERROR(2, "integer or double");
    }
}

static CONTINUATION(op_mul)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) * ARG_INTEGER(3)));
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        call(ARG(1), boxDouble(ARG_DOUBLE(2) * ARG_DOUBLE(3)));
    }
    else
    {
        ARG_ERROR(2, "integer or double");
    }
}

static CONTINUATION(op_div)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) / ARG_INTEGER(3)));
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        call(ARG(1), boxDouble(ARG_DOUBLE(2) / ARG_DOUBLE(3)));
    }
    else
    {
        ARG_ERROR(2, "integer or double");
    }
}

static CONTINUATION(op_floordiv)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) / ARG_INTEGER(3)));
    }
    else if (coercesToDouble(a) && coercesToDouble(b))
    {
        call(ARG(1), boxInteger(ARG_DOUBLE(2) / ARG_DOUBLE(3)));
    }
    else
    {
        ARG_ERROR(2, "integer or double");
    }
}

static CONTINUATION(op_modulus)
{
    value_t a = ARG(2);
    value_t b = ARG(3);
    if (coercesToInteger(a) && coercesToInteger(b))
    {
        call(ARG(1), boxInteger(ARG_INTEGER(2) % ARG_INTEGER(3)));
    }
//  need to figure out nice way to this later on.
//    else if (coercesToDouble(a) && coercesToDouble(b))
//    {
//        call(ARG(1), boxInteger(ARG_DOUBLE(2) % ARG_DOUBLE(3)));
//    }
    else
    {
        ARG_ERROR(2, "integer");
    }
}

static CONTINUATION(to_character)
{
    char ch = ARG_INTEGER(2);
    call(ARG(1), boxString(newString(1, &ch)));
}

static CONTINUATION(to_ordinal)
{
    string_t *a = ARG_STRING(2);
    if (a->length == 0)
    {
        call(ARG(1), boxNull());
    }
    else
    {
        call(ARG(1), boxInteger(a->data[0]));
    }
}

static CONTINUATION(op_and)
{
    size_t i;
    long truth = true;
    for (i = 2; i < argc; i++) truth = truth && ARG_BOOLEAN(i);
    call(ARG(1), boxBoolean(truth));
}

static CONTINUATION(op_or)
{
    size_t i = 2;
    long truth = false;
    for (i = 2; i < argc; i++) truth = truth || ARG_BOOLEAN(i);
    call(ARG(1), boxBoolean(truth));
}

static CONTINUATION(op_not)
{
    call(ARG(1), boxBoolean(!ARG_BOOLEAN(2)));
}

static CONTINUATION(op_lsh)
{
    call(ARG(1), boxInteger(ARG_INTEGER(2) << ARG_INTEGER(3)));
}

static CONTINUATION(op_rsh)
{
    call(ARG(1), boxInteger(ARG_INTEGER(2) >> ARG_INTEGER(3)));
}

static CONTINUATION(op_bit_or)
{
    call(ARG(1), boxInteger(ARG_INTEGER(2) | ARG_INTEGER(3)));
}
static CONTINUATION(op_bit_and)
{
    call(ARG(1), boxInteger(ARG_INTEGER(2) & ARG_INTEGER(3)));
}

static CONTINUATION(op_bit_xor)
{
    call(ARG(1), boxInteger(ARG_INTEGER(2) ^ ARG_INTEGER(3)));
}

static CONTINUATION(op_bit_not)
{
    call(ARG(1), boxInteger(~ARG_INTEGER(2)));
}

static CONTINUATION(op_log)
{
    call(ARG(1), boxDouble(log(ARG_DOUBLE(2))));
}

static CONTINUATION(op_exp)
{
    call(ARG(1), boxDouble(exp(ARG_DOUBLE(2))));
}

static CONTINUATION(op_pow)
{
    call(ARG(1), boxDouble(pow(ARG_DOUBLE(2), ARG_DOUBLE(3))));
}

static CONTINUATION(op_sqrt)
{
    call(ARG(1), boxDouble(sqrt(ARG_DOUBLE(2))));
}

value_t
    v_get_interface,
    v_set_interface,
    v_closure_interface,
    v_numeric_interface,
    v_string_interface,
    v_arraybuffer_interface;

static CONTINUATION(get_interface)
{
    value_t value = ARG(2);
    value_t interface;
    switch (value.type)
    {
        case TYPE_CLOSURE:
            interface = v_closure_interface;
            break;
        case TYPE_BOOLEAN:
        case TYPE_INTEGER:
        case TYPE_DOUBLE:
            interface = v_numeric_interface;
            break;
        case TYPE_STRING:
            interface = v_string_interface;
            break;
        case TYPE_ARRAYBUFFER:
            interface = v_arraybuffer_interface;
            break;
        case TYPE_ARRAY:
            interface = ARG_ARRAY(2)->interface;
            break;
        default:
            assert(false);
    }
    call(ARG(1), interface);
}

static CONTINUATION(set_interface)
{
    array_t *array = ARG_ARRAY(2);
    array->interface = ARG(3);
    call(ARG(1), ARG(2));
}


value_t
    v_call_cc,
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
    v_is_string,
    v_eq, v_ne,
    v_lt, v_le,
    v_gt, v_ge,
    v_add,
    v_sub,
    v_mul,
    v_div,
    v_floordiv,
    v_modulus,
    v_to_character,
    v_to_ordinal,
    v_and,
    v_or,
    v_not,
    v_lsh,
    v_rsh,
    v_bit_or,
    v_bit_and,
    v_bit_xor,
    v_bit_not,
    v_log,
    v_exp,
    v_pow,
    v_sqrt;

value_t uncallable_hook;
value_t type_error_hook;
value_t error_quit;

uint8_t* gc_high;
uint8_t* gc_watermark;

static inline size_t stackSize()
{
    struct rlimit rlimit;
    getrlimit(RLIMIT_STACK, &rlimit);
    return rlimit.rlim_cur;
}

void adjustWaterMark(uint8_t* address)
{
    size_t sz = stackSize();
    size_t pad = 2*1024*1024;
    size_t limit = sz - pad;
    if (sz < pad*2)
    {
        limit = sz / 2;
    }
    gc_high = address;
    gc_watermark = (address - limit);
}

static CONTINUATION(quit)
{
    uint8_t* a = frameAddress();
    printf("used: %li, avail: %li\n", gc_high-a, a-gc_watermark);
    exit(0);
}

static CONTINUATION(errorQuit)
{
    exit(1);
}

void snakeBoot(value_t entry)
{
    adjustWaterMark(frameAddress());

    uncallable_hook = boxNull();
    type_error_hook = boxNull();

    v_call_cc = spawnClosure(call_cc);
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

    v_eq = spawnClosure(op_eq);
    v_ne = spawnClosure(op_ne);
    v_lt = spawnClosure(op_lt);
    v_le = spawnClosure(op_le);
    v_gt = spawnClosure(op_gt);
    v_ge = spawnClosure(op_ge);

    v_add = spawnClosure(op_add);
    v_sub = spawnClosure(op_sub);
    v_mul = spawnClosure(op_mul);
    v_div = spawnClosure(op_div);
    v_floordiv = spawnClosure(op_floordiv);
    v_modulus  = spawnClosure(op_modulus);
    v_to_character = spawnClosure(to_character);
    v_to_ordinal   = spawnClosure(to_ordinal);
    v_and = spawnClosure(op_and);
    v_or  = spawnClosure(op_or);
    v_not = spawnClosure(op_not);

    v_lsh = spawnClosure(op_lsh);
    v_rsh = spawnClosure(op_rsh);
    v_bit_or = spawnClosure(op_bit_or);
    v_bit_and = spawnClosure(op_bit_and);
    v_bit_xor = spawnClosure(op_bit_xor);
    v_bit_not = spawnClosure(op_bit_not);
    v_log = spawnClosure(op_log);
    v_exp = spawnClosure(op_exp);
    v_pow = spawnClosure(op_pow);
    v_sqrt = spawnClosure(op_sqrt);

    v_get_interface = spawnClosure(get_interface);
    v_set_interface = spawnClosure(set_interface);
    v_closure_interface = boxNull();
    v_numeric_interface = boxNull();
    v_string_interface = boxNull();
    v_arraybuffer_interface = boxNull();

    error_quit = spawnClosure(errorQuit);
    call(entry, spawnClosure(quit));
}

void snakeGC(closure_t *closure, size_t argc, value_t *argv)
{
    assert(false);
}
