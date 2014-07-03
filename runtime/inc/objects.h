/*
 * an attempt is done to construct smaller objects of larger
 * fields whenever it is possible.
 *
 * the vm_val can contain any value.
 * the vm_val_array can contain many.
 */
typedef uint64_t vm_val; /* gc_barrier_val */

typedef struct
{
    size_t      used;
    size_t      size;
    vm_val     *vals;
} vm_val_array;

/*
 * everything sitting in heap, accessible as value is an object.
 */
typedef struct
{
    uint64_t    type;
    vm_val      interface;
} vm_object;

typedef enum
{
    vm_t_object = 0,
    vm_t_constant,
    vm_t_integer,
    vm_t_double = 8,
    vm_t_string,
    vm_t_arraybuffer,
    vm_t_dict,
    vm_t_list,
    vm_t_indexing_iterator,
    vm_t_property,
    vm_t_upval,
    vm_t_string_stream,
    vm_t_file_stream,
    vm_t_path,
    vm_t_descriptor,
    vm_t_closure,
    vm_t_stack,
    vm_t_greenlet,
    vm_t_context,
    vm_t_builtin
} vm_object_type;

/*
 * strings and arraybuffers have a lot in common.
 * both are buffers, devoid of pointers.
 */
typedef struct
{
    vm_object   object;
    size_t      length;
    char        data[];
} vm_string;

typedef struct
{
    vm_object   object;
    size_t      length;
    uint8_t     data[];
} vm_arraybuffer;

/*
 * dictionaries and lists
 */
typedef struct
{
    vm_object     object;
    vm_val_array  array;
} vm_dict;

typedef struct 
{
    vm_object     object;
    vm_val_array  array;
} vm_list;

/*
 * everything that has +getitem and .length can be iterated using an indexing iterator.
 */
typedef struct 
{
    vm_object   object;
    vm_val      val;
    size_t      index;
    size_t      length;
} vm_indexing_iterator;

/*
 * there are a lot of objects with property fields, so this is a builtin.
 */
typedef struct
{
    vm_object   object;
    vm_val      get;
    vm_val      set;
    vm_val      call;
} vm_property;

/*
 * upvalues. they never see the stack.
 */
typedef struct 
{
    vm_object   object;
    vm_val      val;
} vm_upval;

/*
 * lets say these three belong to posix.
 */
typedef struct
{
    vm_object       object;
    vm_arraybuffer* buffer;
    int             index;
} vm_string_stream;

typedef struct
{
    vm_object       object;
    int             fd;
} vm_file_stream;

typedef struct vm_path vm_path;

struct vm_path
{
    vm_object       object;
    vm_string      *string;
    vm_path        *parent;
};

/*
 * virtual machine without functions.. what would it be?
 */
typedef struct 
{
    vm_object       object;
    int             flags;      /* varargs or not? */
    size_t          argc;
    size_t          valc;
    size_t          nupvalc;    /* how many new upvalues? */
    vm_arraybuffer *upcopy;     /* interpreted as uint16_t internally */
    vm_arraybuffer *program;
    vm_list        *functions;
    vm_list        *constants;
} vm_descriptor;

/*
 * should be immutable after created.
 */
typedef struct
{
    vm_object       object;
    vm_descriptor  *desc;
    size_t          count;
    vm_upval       *upvalues[];
} vm_closure;

/*
 */
typedef struct
{
    vm_closure     *clos;
    vm_upval      **upvalues;
    vm_val          self;
    int             dst;
    int             base;
    int             top;
    size_t          pc;
    /* alter soon, for C calls */
} vm_frame;

typedef struct
{
    size_t      used;
    size_t      size;
    vm_frame   *frames;
} vm_frame_array;

/*
 */
typedef struct
{
    int         frame;
    int         type;
    int         base;
    size_t      pc;
} vm_except;

typedef struct
{
    size_t      used;
    size_t      size;
    vm_except  *excepts;
} vm_except_array;

/*
 *
 */
typedef struct vm_stack vm_stack;

struct vm_stack
{
    vm_object       object;
    vm_stack*       parent;
    int             status;
    vm_val_array    vals;
    vm_frame_array  frames;
    vm_except_array excepts;
};

/*
 * whereas stack might be an active generator, greenlets are here for
 * async io without the ills.
 */
typedef struct vm_greenlet vm_greenlet;

struct vm_greenlet
{
    vm_object       object;
    vm_greenlet    *parent;
    vm_stack       *stack;
};

/*
 * ultimately we have a context.
 * it is not an object, simply because it's not sitting in heap.
 */
typedef struct
{
    vm_stack       *stack;
    vm_greenlet    *greenlet;
    jmp_buf         exception_return;
    vm_val          exception_type;
    vm_val          exception_value;
    /* vm_list     *exception_traceback; */
} vm_context;

/*
 */

typedef void (*vm_builtin_func)(vm_context* ctx);
typedef struct
{
    vm_object       object;
    const char*     name;
    vm_builtin_func func;
} vm_builtin;

/*
 * might be deprecated, for the VM the nature of the object matters, not it's interface.
struct vm_typespec
{
    size_t   size;
    vm_value interface;
    vm_value (*factory)();
};
*/

/* this is a stub, it belongs elsewhere */
void            vm_panic_(vm_context *ctx, const char *file, const char *func, int line);
#define vm_panic(ctx) vm_panic_((ctx), __FILE__, __func__, __LINE__);

extern vm_val vm_null;
int vm_is_null(vm_val val);
int vm_is_false(vm_val val);
int vm_is_true(vm_val val);
vm_val vm_box(vm_context *ctx, void *object);
vm_val vm_box_bool(vm_context *ctx, int boolean);
vm_val vm_box_integer(vm_context *ctx, int64_t integer);
vm_val vm_box_double(vm_context *ctx, double value);
vm_object_type vm_typeof(vm_val val);
void*   vm_unbox(vm_context *ctx, vm_val val, vm_object_type type);
void*   vm_unbox_object(vm_context *ctx, vm_val val);
int     vm_unbox_bool(vm_context *ctx, vm_val val);
int64_t vm_unbox_integer(vm_context *ctx, vm_val val);
double  vm_unbox_double(vm_context *ctx, vm_val val);
