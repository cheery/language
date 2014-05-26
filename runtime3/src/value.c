#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"

#define NAN_MASK  0xFFF8000000000000L
#define DATA_MASK 0x0000FFFFFFFFFFFFL
#define FIXNUM_SIGN 0x0000800000000000L
#define TAGDATA(tag, data) ((vm_value)(tag) << 48 | ((vm_value)(data) & DATA_MASK))
#define UNTAG(value) ((value) & DATA_MASK)

vm_tag vm_unbox_tag(vm_value value)
{
    vm_tag tag = value >> 48;
    return tag>=vm_tag_double ? vm_tag_double : tag;
}

int vm_true(vm_value value)
{
    return !vm_false(value);
}

int vm_false(vm_value value)
{
    return value == TAGDATA(vm_tag_constant, 0) || value == TAGDATA(vm_tag_object, 0);
}

vm_value vm_box_object(void* object)
{
    return TAGDATA(vm_tag_object, object);
}

void* vm_unbox_object(vm_value value)
{
    return (void*)UNTAG(value);
}

vm_value vm_box_boolean(int boolean)
{
    return TAGDATA(vm_tag_constant, boolean ? 1 : 0);
}

int vm_unbox_boolean(vm_value value)
{
    return UNTAG(value);
}

union pun
{
    vm_value value;
    double number;
};

vm_value vm_box_double(double value)
{
    union pun pun;
    pun.number = value;
    return pun.value - NAN_MASK;
}

double vm_unbox_double(vm_value value)
{
    union pun pun;
    pun.value = value + NAN_MASK;
    return pun.number;
}

typedef struct
{
    size_t length;
    char   string[];
} vm_string;

vm_value vm_box_cstring(const char* string)
{
    return vm_box_string(strlen(string), string);
}

vm_value vm_box_string(size_t length, const char* string)
{
    vm_string* data;

    data = malloc(sizeof(vm_string) + length + 1);
    data->length = length;
    memcpy(data->string, string, length);
    data->string[length] = 0;
    return TAGDATA(vm_tag_string, data);
}

char* vm_unbox_string(vm_value string)
{
    return ((vm_string*)UNTAG(string))->string;
}

size_t vm_string_length(vm_value string)
{
    return ((vm_string*)UNTAG(string))->length;
}

vm_value   vm_box_apifunc(vm_apifunc func)
{
    return TAGDATA(vm_tag_apifunc, func);
}

vm_apifunc vm_unbox_apifunc(vm_value func)
{
    return UNTAG(func);
}

/*

int isNull(Value value)
{
    return value == 0L;
}

Value boxFixnum(long fixnum)
{
    return TAGDATA(AFixnum, fixnum);
}

long unboxFixnum(Value value)
{
    return (value & DATA_MASK) | ((value & FIXNUM_SIGN)?(~DATA_MASK):0);
}
*/
