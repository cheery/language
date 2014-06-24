#include <stdlib.h>
#include "api.h"

static vm_value type_factory()
{
    vm_value basetype;
    vm_object* dict;

    vm_type.size = vm_dict_sizeof();
    dict = malloc(vm_type.size);
    basetype = vm_dict_init(dict);
    dict->interface = basetype;

    return basetype;
}

vm_typespec vm_type = {0, 0, type_factory};

vm_value vm_new_type()
{
    vm_object* dict;

    dict = vm_instantiate(&vm_type, 0);
    return vm_dict_init(dict);
}

void vm_type_method(vm_value type, const char* name, vm_apifunc func)
{
    vm_type_setitem(type, vm_box_cstring(name), vm_box_apifunc(func));
}
