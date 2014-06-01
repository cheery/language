#include <stdlib.h>
#include <stdio.h>
#include "api.h"

vm_value vm_new_api(vm_lib* lib)
{
    vm_value dict;

    dict = vm_new_dict();
    if (lib) while (lib->name)
    {
        vm_dict_setitem(dict, vm_box_cstring(lib->name), vm_box_apifunc(lib->func));
        lib++;
    }
    return dict;
}

vm_value vm_load_baselib();

vm_value vm_init()
{
    return vm_load_baselib();
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

void* vm_get_self_object(vm_context* ctx, vm_typespec *spec)
{
    if (vm_get_interface(ctx->cself) == vm_typespec_interface(spec))
    {
        return vm_unbox_object(ctx->cself);
    }
    else
    {
        VM_STUB();
    }
}

void* vm_get_object(vm_context* ctx, int index, vm_typespec *spec)
{
    vm_value value;

    if (ctx->cbase+index < ctx->ctop)
    {
        value = ctx->cbase[index];
        if (vm_get_interface(value) == vm_typespec_interface(spec))
        {
            return vm_unbox_object(value);
        }
        else
        {
            VM_STUB();
        }
    }
    VM_STUB();
}

void     vm_return_value(vm_context* ctx, vm_value value)
{
    ctx->cbase[-1] = value;
}

void vm_init_typespec(vm_typespec* spec)
{
    if (spec->interface == vm_box_object(NULL))
    {
        spec->interface = spec->factory();
    }
}

void* vm_instantiate(vm_typespec* spec, size_t extra_size)
{
    vm_object* object;

    vm_init_typespec(spec);
    object = malloc(spec->size + extra_size);
    object->interface = spec->interface;
    return object;
}

int vm_has_interface(vm_value value, vm_typespec* spec)
{
    vm_object* object;

    object = vm_unbox_object(value);
    vm_init_typespec(spec);
    return object->interface == spec->interface;
}

static vm_value string_factory()
{
    vm_value type;

    type = vm_new_type();
    return type;
}

vm_typespec vm_string_type = {0, 0, string_factory};

static vm_value apifunc_factory()
{
    vm_value type;

    type = vm_new_type();
    return type;
}

vm_typespec vm_apifunc_type = {0, 0, apifunc_factory};

vm_value vm_get_interface(vm_value value)
{
    switch (vm_unbox_tag(value))
    {
        case vm_tag_object:
            if (value == 0) return 0;
            return ((vm_object*)vm_unbox_object(value))->interface;
        case vm_tag_string:
            return vm_typespec_interface(&vm_string_type);
        case vm_tag_apifunc:
            return vm_typespec_interface(&vm_apifunc_type);
        default:
            printf("object tag: %i\n", vm_unbox_tag(value));
            VM_STUB();
    }
}

vm_value vm_typespec_interface(vm_typespec *spec)
{
    vm_init_typespec(spec);
    return spec->interface;
}

void vm_stub(const char* file, const char* function, int line)
{
    printf("%s:%i: %s stub\n", file, line, function);
    exit(EXIT_FAILURE);
}

void vm_api_register(vm_value lib, const char* name, vm_value value)
{
    vm_dict_setitem(lib, vm_box_cstring(name), value);
}

typedef struct
{
    vm_object object;
    vm_value get, set, call;
} vm_property;

static vm_value property_factory()
{
    vm_value type;
    
    type = vm_new_type();
    return type;
}
vm_typespec vm_property_type = {sizeof(vm_property), 0, property_factory};

vm_value vm_new_property(vm_value get, vm_value set, vm_value call)
{
    vm_property* property;

    property = vm_instantiate(&vm_property_type, 0);
    property->get  = get;
    property->set  = set;
    property->call = call;
    return vm_box_object(property);
}

vm_value vm_property_get_get(vm_value value)
{
    vm_property* property;

    property = vm_unbox_object(value);
    return property->get;
}

vm_value vm_property_get_set(vm_value value)
{
    vm_property* property;

    property = vm_unbox_object(value);
    return property->set;
}

vm_value vm_property_get_call(vm_value value)
{
    vm_property* property;

    property = vm_unbox_object(value);
    return property->call;
}
