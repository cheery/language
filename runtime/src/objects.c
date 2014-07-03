#include "vm.h"
#include <string.h>

#include <error.h>
#include <stdio.h>
void vm_panic_(vm_context *ctx, const char *file, const char *func, int line)
{
    fprintf(stderr, "%s:%i (%s)\n", file, line, func);
    error(1, 0, "panic");
}

#define NAN_MASK  0xFFF8000000000000L
#define DATA_MASK 0x0000FFFFFFFFFFFFL
#define FIXNUM_SIGN 0x0000800000000000L
#define tagdata(type, data) ((vm_val)(((uint64_t)(type) << 48) | ((data) & DATA_MASK)))

static vm_object_type valtag(vm_val val)
{
    vm_object_type type = val >> 48;
    return type >= vm_t_double ? vm_t_double : type;
}

static uint64_t valdata(vm_val val)
{
    return val & DATA_MASK;
}

vm_val vm_null = tagdata(vm_t_object, 0);

int vm_is_null(vm_val val)
{
    return val == tagdata(vm_t_object, 0);
}

int vm_is_false(vm_val val)
{
    return val == tagdata(vm_t_object, 0) || val == tagdata(vm_t_constant, 0);
}

int vm_is_true(vm_val val)
{
    return !vm_is_false(val);
}

vm_val vm_box(vm_context *ctx, void *object)
{
    return tagdata(vm_t_object, (uint64_t)object);
}

vm_val vm_box_bool(vm_context *ctx, int boolean)
{
    return tagdata(vm_t_constant, boolean ? 1 : 0);
}

vm_val vm_box_integer(vm_context *ctx, int64_t integer)
{
    return tagdata(vm_t_integer, integer);
}

vm_val vm_box_double(vm_context *ctx, double value)
{
    union pun { uint64_t value; double number; } pun;
    pun.number = value;
    return pun.value - NAN_MASK;
}

vm_object_type vm_typeof(vm_val val)
{
    vm_object       *object;
    vm_object_type   tag;

    tag = valtag(val);
    if (tag == vm_t_object)
    {
        object = (vm_object*)valdata(val);
        if (object)
        {
            return object->type;
        }
    }
    return tag;
}

void* vm_unbox(vm_context *ctx, vm_val val, vm_object_type type)
{
    if (vm_typeof(val) != type)
    {
        vm_panic(ctx);
    }
    return (void*)valdata(val);
}

void* vm_unbox_object(vm_context *ctx, vm_val val)
{
    return (void*)valdata(val);
}

int vm_unbox_bool(vm_context *ctx, vm_val val)
{
    if (val == tagdata(vm_t_constant, 0))
    {
        return 0;
    }
    else if (val == tagdata(vm_t_constant, 1))
    {
        return 1;
    }
    else
    {
        vm_panic(ctx);
        return 0;
    }
}

int64_t vm_unbox_integer(vm_context *ctx, vm_val val)
{
    uint64_t data;
    if (valtag(val) != vm_t_integer)
    {
        vm_panic(ctx);

    }
    data = valdata(val);
    return data | ((data & FIXNUM_SIGN)?(~DATA_MASK):0);
}

double vm_unbox_double(vm_context *ctx, vm_val val)
{
    union pun { uint64_t value; double number; } pun;
    if (valtag(val) != vm_t_double)
    {
        vm_panic(ctx);
    }

    pun.value = valdata(val) + NAN_MASK;
    return pun.number;
}
