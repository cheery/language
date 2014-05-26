#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "value.h"
#include "list.h"

typedef struct 
{
    vm_object object;
    size_t    length;
    size_t    size;
    vm_value *values;
} vm_list;

vm_value vm_new_list()
{
    vm_list* list;
    
    list = malloc(sizeof(*list));
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
