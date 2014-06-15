#include <stdlib.h>
#include "api.h"

static void greenlet_switch(vm_context* ctx)
{
    vm_greenlet* next;

    next = vm_get_self_object(ctx, &vm_greenlet_type);

    while(next && vm_stack_dead(next->stack))
    {
        next = next->parent;
    }

    if (next == NULL)
    {
        VM_STUB();
    }

    vm_stack_resume_greenlet(ctx, next);
}

static void greenlet_dead(vm_context* ctx)
{
    vm_greenlet* greenlet;

    greenlet = vm_get_self_object(ctx, &vm_greenlet_type);
    vm_return_value(ctx, vm_box_boolean(vm_stack_dead(greenlet->stack)));
}

static void greenlet_parent(vm_context* ctx)
{
    vm_greenlet* greenlet;

    greenlet = vm_get_self_object(ctx, &vm_greenlet_type);
    vm_return_value(ctx, vm_box_object(greenlet->parent));
}

static vm_value greenlet_factory()
{
    vm_value type;
    
    type = vm_new_type();
    vm_type_method(type, "switch", greenlet_switch);
    vm_type_setitem(type, vm_box_cstring("dead"),   vm_new_property(vm_box_apifunc(greenlet_dead),   0, 0));
    vm_type_setitem(type, vm_box_cstring("parent"), vm_new_property(vm_box_apifunc(greenlet_parent), 0, 0));
    return type;
}

vm_typespec vm_greenlet_type = {sizeof(vm_greenlet), 0, greenlet_factory};

vm_greenlet* vm_new_greenlet(vm_greenlet* parent, vm_value self, int argc, vm_value* argv)
{
    vm_greenlet* greenlet;

    greenlet = vm_instantiate(&vm_greenlet_type, 0);
    greenlet->parent = parent;
    greenlet->stack  = vm_new_stack(self, argc, argv);
    return greenlet;
}
