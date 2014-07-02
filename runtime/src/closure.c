#include "vm.h"

vm_descriptor* vm_new_descriptor(
        vm_context     *ctx,
        int             flags,
        size_t          argc,
        size_t          valc,
        size_t          nupvalc,
        vm_arraybuffer *upcopy,
        vm_arraybuffer *program,
        vm_list        *functions,
        vm_list        *constants)
{
    vm_descriptor *desc;

    desc = gc_new(ctx, sizeof(vm_descriptor), vm_t_descriptor, interface_stub);
    desc->argc      = argc;
    desc->valc      = valc;
    desc->nupvalc   = nupvalc;
    desc->upcopy    = upcopy;
    desc->program   = program;
    desc->functions = functions;
    desc->constants = constants;
    return desc;
}

vm_closure* vm_new_closure(vm_context *ctx, vm_descriptor *desc)
{
    vm_closure* closure;
    size_t count = vm_arraybuffer_count(desc->upcopy, uint16_t);

    if (count > 0)
    {
        vm_panic(ctx);
    }

    closure = gc_new(ctx, sizeof(vm_closure) + sizeof(vm_upval*)*count, vm_t_closure, interface_stub);
    closure->desc  = desc;
    closure->count = count;
    return closure;
}
