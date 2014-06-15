#include <stdlib.h>
#include <string.h>
#include "api.h"

static vm_value descriptor_factory()
{
    vm_value type;
    
    type = vm_new_type();
    return type;
}

vm_typespec vm_descriptor_type = {sizeof(vm_descriptor), 0, descriptor_factory};

static vm_value closure_factory()
{
    vm_value type;
    
    type = vm_new_type();
    return type;
}

vm_typespec vm_closure_type = {sizeof(vm_closure), 0, closure_factory};

vm_descriptor* vm_new_descriptor(
        size_t argc,
        size_t valc,
        size_t upvalc,
        size_t upcopyc,
        size_t size,
        uint16_t* upcopy,
        uint8_t* program,
        vm_value functions,
        vm_value constants)
{
    vm_descriptor* desc;

    desc = vm_instantiate(&vm_descriptor_type, 0);
    desc->argc = argc;
    desc->valc = valc;
    desc->upvalc = upvalc;
    desc->upcopyc = upcopyc;
    desc->upcopy = realloc(NULL, sizeof(*upcopy) * upcopyc);
    memcpy(desc->upcopy, upcopy, sizeof(*upcopy) * upcopyc);
    desc->size = size;
    desc->program = realloc(NULL, size);
    memcpy(desc->program, program, size);
    desc->functions = functions;
    desc->constants = constants;
    return desc;
}

vm_closure* vm_new_closure(
        vm_descriptor* desc,
        vm_value** upvalues)
{
    vm_closure* closure;
    int i;

    closure = vm_instantiate(&vm_closure_type, sizeof(*upvalues) * desc->upcopyc);
    closure->desc = desc;
    for(i = 0; i < desc->upcopyc; i++)
    {
        closure->upvalues[i] = upvalues[desc->upcopy[i]];
    }
    return closure;
}
