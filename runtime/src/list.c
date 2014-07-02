#include "vm.h"

vm_list* vm_new_list(vm_context *ctx, vm_val interface)
{
    vm_list *list;

    list = gc_new(ctx, sizeof(vm_list), vm_t_list, interface);
    list->array.used = 0;
    list->array.size = 0;
    list->array.vals = NULL;
    return list;
}

void vm_list_append(vm_context *ctx, vm_list* list, vm_val item)
{
    vm_val_array *array = &list->array;
    size_t size;

    if (array->used >= array->size)
    {
        size = array->size*2 + 8;
        array->vals = gc_realloc(ctx, array->vals, sizeof(vm_val) * size);
        array->size = size;
    }
    gc_barrier_val(ctx, list, array->vals[array->used++] = item);
}

size_t vm_list_length(vm_context *ctx, vm_list* list)
{
    return list->array.used;
}

vm_val vm_list_getitem(vm_context *ctx, vm_list* list, int index)
{
    if (index >= list->array.used)
    {
        vm_panic(ctx);
    }
    return list->array.vals[index];
}

void vm_list_setitem(vm_context *ctx, vm_list* list, int index, vm_val item)
{
    if (index >= list->array.used)
    {
        vm_panic(ctx);
    }
    gc_barrier_val(ctx, list, list->array.vals[index] = item);
}
