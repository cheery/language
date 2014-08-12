#include <alloca.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t* gc_watermark;

// should adjust this for non-gcc compilers
#define frameAddress() ((uint8_t*)__builtin_frame_address(0))

#define checkWaterMark() \
    if (frameAddress() < gc_watermark) \
    { \
        snakeGC(frame, argc, argv); \
        return; \
    }

#define noreturn
#define CONTINUATION(name) void name(closure_t *frame, size_t argc, value_t *argv)
#define ARG(index) (index<argc?argv[index]:boxNull())
#define SLOT(index) (closureSlot(frame, index))

#define ARG_ERROR(index, expected) \
    argError(frame, argc, argv, index, expected);
#define ARG_CLOSURE(index) \
    ({ value_t a = ARG(index); if(!isClosure(a)) ARG_ERROR(index, "closure"); unboxClosure(a);})
#define ARG_BOOLEAN(index) \
    ({ value_t a = ARG(index); unboxBoolean(a);})
#define ARG_INTEGER(index) \
    ({ value_t a = ARG(index); if(!coercesToInteger(a)) ARG_ERROR(index, "integer"); unboxInteger(a);})
#define ARG_DOUBLE(index) \
    ({ value_t a = ARG(index); if(!coercesToDouble(a) && !isBoolean(a)) ARG_ERROR(index, "double"); unboxDouble(a);})
#define ARG_STRING(index) \
    ({ value_t a = ARG(index); if(!isString(a)) ARG_ERROR(index, "string"); unboxString(a);})
#define ARG_ARRAYBUFFER(index) \
    ({ value_t a = ARG(index); if(!isArrayBuffer(a)) ARG_ERROR(index, "arraybuffer"); unboxArrayBuffer(a);})
#define ARG_ARRAY(index) \
    ({ value_t a = ARG(index); if(!isArray(a)) ARG_ERROR(index, "array"); unboxArray(a);})


typedef struct value        value_t;
typedef struct object       object_t;
typedef struct array        array_t;
typedef struct arraybuffer  arraybuffer_t;
typedef struct closure      closure_t;
typedef struct string       string_t;
// integer is long
// double  is double
typedef void (*proc_t)(closure_t* frame, size_t argc, value_t *argv) noreturn;

/*
 * value cell.
 */
struct value
{
    uint8_t type;
    union {
        long        integer;
        double      real;
        object_t   *object;
        void*       address;
    } a;
};

#define TYPE_CLOSURE     0
#define TYPE_BOOLEAN     1
#define TYPE_INTEGER     2
#define TYPE_DOUBLE      3
#define TYPE_STRING      4
#define TYPE_ARRAYBUFFER 5
#define TYPE_ARRAY       7

struct object
{
};

struct array
{
    object_t    object;
    value_t     interface;
    size_t      length;
    value_t     val[];
};

struct arraybuffer
{
    object_t    object;
    size_t      length;
    uint8_t     data[];
};

struct closure
{
    object_t    object;
    proc_t      proc;
    size_t      length;
    value_t*    slot[];
};

struct string
{
    object_t    object;
    size_t      byteLength;
    size_t      length;
    uint8_t     data[];
};

static inline value_t boxNull()
{
    value_t value   = {TYPE_CLOSURE};
    value.a.address = NULL;
    return value;
}

static inline value_t boxTrue()
{
    value_t value   = {TYPE_BOOLEAN};
    value.a.integer = true;
    return value;
}

static inline value_t boxFalse()
{
    value_t value   = {TYPE_BOOLEAN};
    value.a.integer = false;
    return value;
}

static inline value_t boxBoolean(long integer)
{
    value_t value   = {TYPE_BOOLEAN};
    value.a.integer = integer;
    return value;
}

static inline value_t boxInteger(long integer)
{
    value_t value   = {TYPE_INTEGER};
    value.a.integer = integer;
    return value;
}

static inline value_t boxDouble(double real)
{
    value_t value   = {TYPE_DOUBLE};
    value.a.real    = real;
    return value;
}

static inline value_t boxArray(array_t *array)
{
    value_t value   = {TYPE_ARRAY};
    value.a.address = array;
    return value;
}

static inline value_t boxArrayBuffer(arraybuffer_t *array)
{
    value_t value   = {TYPE_ARRAYBUFFER};
    value.a.address = array;
    return value;
}

static inline value_t boxClosure(closure_t *closure)
{
    value_t value   = {TYPE_CLOSURE};
    value.a.address = closure;
    return value;
}

static inline value_t boxString(string_t *string)
{
    value_t value   = {TYPE_STRING};
    value.a.address = string;
    return value;
}

static inline long isNull(value_t value)
{
    return value.type == TYPE_CLOSURE && value.a.address == NULL;
}

static inline long isBoolean(value_t value)
{
    return value.type == TYPE_BOOLEAN;
}

static inline long isTrue(value_t value)
{
    return value.type == TYPE_BOOLEAN && value.a.integer;
}

static inline long isFalse(value_t value)
{
    return value.type == TYPE_BOOLEAN && !value.a.integer;
}

static inline long isInteger(value_t value)
{
    return value.type == TYPE_INTEGER;
}

static inline long isDouble(value_t value)
{
    return value.type == TYPE_DOUBLE;
}

static inline long isArray(value_t value)
{
    return value.type == TYPE_ARRAY;
}

static inline long isArrayBuffer(value_t value)
{
    return value.type == TYPE_ARRAYBUFFER;
}

static inline long isClosure(value_t value)
{
    return value.type == TYPE_CLOSURE && value.a.address != NULL;
}

static inline long isString(value_t value)
{
    return value.type == TYPE_STRING;
}

static inline long coercesToInteger(value_t value)
{
    return value.type == TYPE_INTEGER || value.type == TYPE_BOOLEAN;
}

static inline long coercesToDouble(value_t value)
{
    return value.type == TYPE_DOUBLE || value.type == TYPE_INTEGER || value.type == TYPE_BOOLEAN;
}

static inline array_t *initArray(array_t *array, size_t length)
{
    size_t i;
    array->length = length;
    for (i = 0; i < length; i++) array->val[i] = boxNull();
    return array;
}

static inline arraybuffer_t *initArrayBuffer(arraybuffer_t *array, size_t length)
{
    size_t i;
    array->length = length;
    for (i = 0; i < length; i++) array->data[i] = 0;
    return array;
}

/*
 * invariant: every cell is subsequently filled with unique slot.
 */
static inline closure_t *initClosure(closure_t *closure, proc_t proc, size_t length, value_t* slot[])
{
    size_t i;
    closure->proc   = proc;
    closure->length = length;
    if (slot) for (i = 0; i < length; i++) closure->slot[i] = slot[i];
    return closure;
}

static inline string_t *initString(string_t *string, size_t length, const char *data)
{
    size_t i;
    string->byteLength = length+1;
    string->length = length;
    if (data) for (i = 0; i < length; i++) string->data[i] = data[i];
    string->data[length] = 0;
    return string;
}

/*
 * invariant: type is checked before these are used.
 */
static inline long unboxBoolean(value_t value)
{
    return !(isFalse(value) || isNull(value));
}

static inline long unboxInteger(value_t value)
{
    if (isBoolean(value)) return (double)(value.a.integer?1:0); // implicit coercion
    return value.a.integer;
}

static inline double unboxDouble(value_t value)
{
    if (isBoolean(value)) return (double)(value.a.integer?1:0); // implicit coercion
    if (isInteger(value)) return (double)value.a.integer; // implicit coercion
    return value.a.real;
}

static inline array_t* unboxArray(value_t value)
{
    return value.a.address;
}

static inline arraybuffer_t* unboxArrayBuffer(value_t value)
{
    return value.a.address;
}

static inline closure_t* unboxClosure(value_t value)
{
    return value.a.address;
}

static inline string_t* unboxString(value_t value)
{
    return value.a.address;
}

/*
 * invariant: continuations never return
 */
#define allocav(x, y, n)    (alloca(sizeof(x) + sizeof(y)*(n)))
#define newArray(n)         (initArray(allocav(array_t, value_t, n), n))
#define newArrayBuffer(n)   (initArrayBuffer(allocav(arraybuffer_t, uint8_t, n), n))
#define newCString(x)       (initString(allocav(string_t, char, strlen(x)+1), strlen(x), x))
#define newString(l, x)     (initString(allocav(string_t, char, l+1), l, x))

#define spawnString(x)      (boxString(newCString(x)))
#define spawnClosure(proc, slots...) ({ \
        value_t* _slots[] = {slots}; \
        size_t   n = sizeof(_slots) / sizeof(value_t*); \
        boxClosure(initClosure(allocav(closure_t, value_t*, n), proc, n, _slots)); \
        })

#define call(args...) ({ \
    value_t _args[] = {args}; \
    snakeCall(sizeof(_args)/sizeof(value_t), _args); \
    })

//static inline void snakeCall(size_t argc, value_t *args) c_noreturn;


/*
 * invariant: errors go into debugger, so assert.h will fuck off very soon.
 */
#include <assert.h>

extern value_t uncallable_hook;
extern value_t type_error_hook;
extern value_t error_quit;

/*
 * invariant: we have at least a callee.
 */
static inline void snakeCall(size_t argc, value_t *argv)
{
    if (isClosure(argv[0]))
    {
        closure_t *frame = unboxClosure(argv[0]);
        frame->proc(frame, argc, argv);
    }
    else
    {
        if (isClosure(uncallable_hook))
        {
            call(uncallable_hook, error_quit, argv[0]);
        }
        else
        {
            assert(false);
        }
    }
}

static inline void argError(closure_t *closure, size_t argc, value_t *argv, size_t index, const char* expected)
{
    if (isClosure(type_error_hook))
    {
        call(type_error_hook, error_quit, boxInteger(index), spawnString(expected));
    }
    else
    {
        assert(false);
    }
}


/*
 * invariant: the sources alter the slots only through these functions.
 */
static inline value_t* closureSlot(closure_t *closure, size_t index)
{
    return closure->slot[index];
} 

/*
 * invariant: we <3 puns.
 */
void snakeBoot(value_t entry);
void snakeGC(closure_t *closure, size_t argc, value_t *argv);
