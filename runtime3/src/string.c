#include <stdlib.h>
#include "api.h"

typedef struct 
{
    vm_object object;
    vm_value  string;
    size_t    index;
} vm_string_iterator;

static vm_value string_iterator_factory();
vm_typespec vm_string_iterator_type = {sizeof(vm_string_iterator), 0, string_iterator_factory};

static void string_iterator_next(vm_context* ctx)
{
    vm_string_iterator* iterator;
    char* string;
    size_t length;

    iterator = vm_get_self_object(ctx, &vm_string_iterator_type);
    length = vm_string_length(iterator->string);
    string = vm_unbox_string(iterator->string);
    if (iterator->index < length)
    {
        vm_return_value(ctx, vm_box_string(1, &string[iterator->index++]));
    }
    else
    {
        vm_stopiteration(ctx);
    }
}

static vm_value string_iterator_factory()
{
    vm_value type;

    type = vm_new_type();
    vm_type_method(type, "+next", string_iterator_next);
    return type;
}

static void new_iterator(vm_context* ctx)
{
    vm_string_iterator* iterator;

    iterator = vm_instantiate(&vm_string_iterator_type, 0);
    vm_get_self_object(ctx, &vm_string_type);
    iterator->string = vm_get_self(ctx);
    iterator->index  = 0;
    vm_return_value(ctx, vm_box_object(iterator));
}

static vm_value string_factory()
{
    vm_value type;

    type = vm_new_type();
    vm_type_method(type, "+iter", new_iterator);
    return type;
}

vm_typespec vm_string_type = {0, 0, string_factory};
