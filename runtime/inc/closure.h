/*typedef struct vm_descriptor vm_descriptor;
typedef struct vm_closure    vm_closure;

extern vm_typespec vm_descriptor_type;
extern vm_typespec vm_closure_type;

struct vm_descriptor
{
    vm_object object;
    size_t argc;
    size_t valc;
    size_t upvalc;
    size_t upcopyc;
    uint16_t* upcopy;
    size_t   size;
    uint8_t* program;
    vm_value functions;
    vm_value constants;
};

struct vm_closure
{
    vm_object object;
    vm_descriptor* desc;
    vm_value* upvalues[];
};*/

vm_descriptor* vm_new_descriptor(
        vm_context     *ctx,
        int             flags,
        size_t          argc,
        size_t          valc,
        size_t          nupvalc,
        vm_arraybuffer *upcopy,
        vm_arraybuffer *program,
        vm_list        *functions,
        vm_list        *constants);

/*
 * The context must be in a desirable state for the descriptor.
 */
vm_closure* vm_new_closure(vm_context *ctx, vm_descriptor *desc);
