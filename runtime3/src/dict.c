#include <stdint.h>
#include <stdlib.h>
#include "gc.h"
#include "value.h"
#include "dict.h"
#include "cmp.h"

typedef struct
{
    vm_value key;
    vm_value value;
} vm_dict_entry;

typedef struct
{
    vm_object object;
    size_t size;
    size_t length;
    vm_dict_entry *entries;
} vm_dict;

vm_value vm_new_dict()
{
    vm_dict* dict;

    dict = malloc(sizeof(*dict));
    dict->size   = 0;
    dict->length = 0;
    dict->entries = NULL;
    return vm_box_object(dict);
}

size_t   vm_dict_length(vm_value dict)
{
    return ((vm_dict*)vm_unbox_object(dict))->length;
}

int vm_dict_getitem(vm_value object, vm_value key, vm_value* item)
{
    vm_dict* dict;
    int i;

    dict = vm_unbox_object(object);

    for (i = 0; i < dict->length; i++)
    {
        if (vm_cmp(dict->entries[i].key, key) == 0)
        {
            *item = dict->entries[i].value;
            return 1;
        }
    }
    *item = vm_box_object(0);
    return 0;
}

void     vm_dict_setitem(vm_value object, vm_value key, vm_value item)
{
    vm_dict* dict;
    int i;

    dict = vm_unbox_object(object);

    for (i = 0; i < dict->length; i++)
    {
        if (vm_cmp(dict->entries[i].key, key) == 0)
        {
            dict->entries[i].value = item;
            return;
        }
    }

    if (dict->length >= dict->size)
    {
        dict->size = dict->size*2 + 8;
        dict->entries = realloc(dict->entries, sizeof(vm_dict_entry)*dict->size);
    }
    dict->entries[dict->length].key   = key;
    dict->entries[dict->length].value = item;
    dict->length++;
}
