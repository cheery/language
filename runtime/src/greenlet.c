#include "vm.h"

vm_greenlet* vm_new_greenlet(vm_context* ctx, vm_greenlet* parent, vm_val self, int argc, vm_val* argv)
{
    vm_greenlet *greenlet;

    greenlet = gc_new(ctx, sizeof(vm_greenlet), vm_t_greenlet, interface_stub);
    greenlet->parent = parent;
    greenlet->stack  = vm_new_stack(ctx, self, argc, argv);
    return greenlet;
} 
