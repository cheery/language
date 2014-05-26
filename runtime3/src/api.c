#include <stdlib.h>
#include "api.h"

vm_value vm_new_api(vm_lib* lib)
{
    vm_value dict;

    dict = vm_new_dict();
    while (lib->name)
    {
        vm_dict_setitem(dict, vm_box_cstring(lib->name), vm_box_apifunc(lib->func));
        lib++;
    }
    return dict;
}

extern vm_lib vm_base_library[];

vm_value vm_init()
{
    return vm_new_api(vm_base_library);
}

size_t   vm_get_argc(vm_context* ctx)
{
    return ctx->ctop - ctx->cbase;
}

vm_value vm_get_value(vm_context* ctx, int index)
{
    if (ctx->cbase+index < ctx->ctop) return ctx->cbase[index];
    return vm_box_object(NULL);
}

void     vm_return_value(vm_context* ctx, vm_value value)
{
    ctx->cbase[-1] = value;
}
