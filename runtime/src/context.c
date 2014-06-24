#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "api.h"

vm_context* vm_new_context()
{
    vm_context* context;

    context = malloc(sizeof(*context));
    context->stack    = NULL;
    context->greenlet = NULL;
    return context;
}

void vm_context_bootup(vm_context* context, vm_closure* closure)
{
    vm_value argv[1] = {vm_box_object(closure)};
    context->greenlet = vm_new_greenlet(NULL, vm_null, 1, argv);
    vm_stack_resume(context, context->greenlet->stack, 0, 0);
}
