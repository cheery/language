#include "vm.h"

vm_dict* vm_new_dict(vm_context *ctx, vm_val interface)
{
    vm_dict *dict;

    dict = gc_new(ctx, sizeof(vm_dict), vm_t_dict, interface);
    dict->array.used = 0;
    dict->array.size = 0;
    dict->array.vals = NULL;
    return dict;
}

size_t vm_dict_length(vm_context *ctx, vm_dict *dict)
{
    return dict->array.used / 2;
}

//typedef struct
//{
//    vm_object     object;
//    vm_val_array  array;
//} vm_dict;

int vm_dict_getitem(vm_context *ctx, vm_dict *dict, vm_val key, vm_val *item)
{
    int i;

    for (i = 0; i < dict->array.used; i+=2)
    {
        if (vm_cmp(ctx, dict->array.vals[i+0], key) == 0)
        {
            *item = dict->array.vals[i+1];
            return 1;
        }
    }
    *item = vm_null;
    return 0;
}

void vm_dict_setitem(vm_context *ctx, vm_dict *dict, vm_val key, vm_val item)
{
    int i;

    for (i = 0; i < dict->array.used; i+=2)
    {
        if (vm_cmp(ctx, dict->array.vals[i], key) == 0)
        {
            gc_barrier_val(ctx, dict, dict->array.vals[i+1] = item);
            return;
        }
    }

    if (dict->array.used >= dict->array.size)
    {
        dict->array.size = dict->array.size*2 + 8;
        dict->array.vals = gc_realloc(ctx, dict->array.vals, sizeof(vm_val)*dict->array.size);
    }
    gc_barrier_val(ctx, dict, dict->array.vals[dict->array.used+0] = key);
    gc_barrier_val(ctx, dict, dict->array.vals[dict->array.used+1] = item);
    dict->array.used+=2;
}
