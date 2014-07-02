#include "vm.h"

void gc_barrier(vm_context *ctx, void *subject, void *object)
{
}

void gc_barrier_val(vm_context *ctx, void *subject, vm_val val)
{
}

void* gc_new(vm_context *ctx, size_t size, vm_object_type type, vm_val interface)
{
    vm_object* object;

    object = malloc(size);
    object->type = type;
    object->interface = interface;
    return object;
}

void* gc_realloc(vm_context *ctx, void *address, size_t size)
{
    return  realloc(address, size);
}
