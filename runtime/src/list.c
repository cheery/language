#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

typedef struct 
{
    vm_object object;
    size_t    length;
    size_t    size;
    vm_value *values;
} vm_list;

static void append(vm_context* ctx)
{
    vm_list* list;

    list = vm_get_self_object(ctx, &vm_list_type);
    vm_list_append(vm_box_object(list), vm_get_value(ctx, 0));
    vm_return_value(ctx, vm_box_object(0));
}

static void get_length(vm_context* ctx)
{
    vm_list* list;

    list = vm_get_object(ctx, 0, &vm_list_type);
    vm_return_value(ctx, vm_box_integer(list->length));
}

static void getitem(vm_context* ctx)
{
    vm_list* list;
    long index;

    list  = vm_get_self_object(ctx, &vm_list_type);
    index = vm_get_integer(ctx, 0);
    if (index >= list->length)
    {
        VM_STUB();
    }
    vm_return_value(ctx, list->values[index]);
}


static vm_value list_factory()
{
    vm_value type;
    
    type = vm_new_type();
    vm_type_method(type, "append", append);
    vm_type_method(type, "+getitem", getitem);
    vm_type_setitem(type, vm_box_cstring("length"), vm_new_property(vm_box_apifunc(get_length), 0, 0));

    return type;
}
vm_typespec vm_list_type = {sizeof(vm_list), 0, list_factory};

vm_value vm_new_list()
{
    vm_list* list;
    
    list = vm_instantiate(&vm_list_type, 0);
    list->length = 0;
    list->size   = 0;
    list->values = NULL;
    return vm_box_object(list);
}

void     vm_list_append(vm_value object, vm_value item)
{
    vm_list* list;
    
    list = vm_unbox_object(object);
    if (list->length >= list->size)
    {
        list->size   = list->size*2 + 8;
        list->values = realloc(list->values, sizeof(*list->values) * list->size);
    }
    list->values[list->length++] = item;
}

size_t   vm_list_length(vm_value object)
{
    vm_list* list;
    
    list = vm_unbox_object(object);
    return list->length;
}

vm_value vm_list_getitem(vm_value object, int index)
{
    vm_list* list;
    
    list = vm_unbox_object(object);
    return list->values[index];
}

void     vm_list_setitem(vm_value object, int index, vm_value item)
{
    vm_list* list;
    
    list = vm_unbox_object(object);
    list->values[index] = item;
}
