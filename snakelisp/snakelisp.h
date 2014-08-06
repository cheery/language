#include <stdlib.h>
#include <alloca.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef struct value        value_t;
typedef struct closure      closure_t;
typedef struct argv         argv_t;
typedef struct string       string_t;
typedef struct arraybuffer  arraybuffer_t;

typedef void (*code_t)(closure_t*, argv_t*);

/*
 * This is a bit complex. This is a value cell.
 * A slot is a single value cell somewhere.
 */
struct value
{
    uint8_t type;
    union {
        long            integer;
        string_t        *string;
        closure_t       *closure;
        arraybuffer_t   *arraybuffer;
    } data;
};
#define TYPE_NULL    0
#define TYPE_BOOLEAN 1
#define TYPE_INTEGER 2
#define TYPE_STRING  3
#define TYPE_ARRAYBUFFER  4
#define TYPE_ARRAY   127
#define TYPE_CLOSURE 128


/*
 * Some basic data types compose complex data types.
 */
struct string
{
    size_t  length;
    char    data[];
};

struct arraybuffer
{
    size_t  length;
    uint8_t data[];
};

/*
 * This is pass by value system, so the argument list is itself
 * sequence of slots.
 *
 * The closure is a sequence of pointers to slots.
 */

struct closure
{
    code_t   code;
    size_t   slotz;
    value_t* slot[];
};

struct argv
{
    size_t   valz;
    value_t  val[];
};

static inline value_t c_const_null()
{
    value_t val = {TYPE_NULL, 0};
    return val;
}

static inline value_t c_const_integer(long integer)
{
    value_t val = {TYPE_INTEGER};
    val.data.integer = integer;
    return val;
}

static inline value_t c_const_closure(closure_t *closure)
{
    printf("achievement: closure constant %p\n", closure);
    value_t val = {TYPE_CLOSURE};
    val.data.closure = closure;
    return val;
}

static inline value_t c_const_string_init(string_t *string, size_t length, const char *data)
{
    value_t val   = {TYPE_STRING};
    val.data.string = string;
    string->length = length + 1;
    memcpy(string->data, data, length);
    string->data[length] = 0;
    return val;
}

// some implementations might refuse to inline the function if it uses alloca
#define c_const_string(x) c_const_string_init(alloca(sizeof(string_t) + sizeof(char)*(strlen(x)+1)), strlen(x), x);

// for now not sure how it enforces.
#define c_noreturn

#define c_alloc_closure(name, cname, vname, count) \
    closure_t *cname = alloca(sizeof(closure_t) + sizeof(value_t*)*count); \
    cname->code   = name; \
    cname->slotz  = count; \
    value_t vname = c_const_closure(cname);

#define c_call_begin(arity) \
    argv_t *argv = alloca(sizeof(argv_t) + sizeof(value_t)*arity); \
    argv->valz   = arity;

#define c_call_argument(index, value) \
    argv->val[index] = value;

// nothing for now. :)
#define c_call_end() c_call(argv);

/*
 * Arguments are unpacked before used, so we can verify it has a slot even if callee gave too few arguments. 
 */
static inline value_t c_get_argument(argv_t* args, int index)
{
    value_t val = {TYPE_NULL, 0};
    if (index < args->valz) val = args->val[index];
    return val;
}

static inline void c_call(argv_t* args)
{
    closure_t* closure = args->val[0].data.closure;
    printf("achievement: call %p\n", closure);
    assert (args->val[0].type == TYPE_CLOSURE);
    closure->code(closure, args);
}

static inline value_t* c_slot(closure_t *closure, long index)
{
    return closure->slot[index];
}

static inline void c_closure_lift(closure_t *closure, long index, value_t* slot)
{
    closure->slot[index] = slot;
}

value_t cl_pick,
        cl_arraybuffer,
        cl_file_open,
        cl_file_close,
        cl_file_read,
        cl_file_write,
        v_stdin,
        v_stdout,
        v_stderr;

void c_boot(code_t entry);
