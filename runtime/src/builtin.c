#include "vm.h"

vm_builtin* vm_new_builtin(vm_context *ctx, const char* name, vm_builtin_func func)
{
    vm_builtin* builtin;

    builtin = gc_new(ctx, sizeof(vm_builtin), vm_t_builtin, interface_stub);
    builtin->name = name;
    builtin->func = func;
    return builtin;
}

size_t vm_c_argc(vm_context *ctx)
{
    vm_frame *frame;

    frame = vm_stack_current_frame(ctx, ctx->stack);
    return frame->pc;
}

vm_val vm_c_load(vm_context *ctx, size_t index)
{
    vm_frame *frame;

    frame = vm_stack_current_frame(ctx, ctx->stack);
    if (index >= ctx->stack->vals.used)
    {
        return vm_null;
    }
    return ctx->stack->vals.vals[frame->base + index];
}

void vm_c_return(vm_context *ctx, vm_val val)
{
    int dst;

    dst = vm_stack_current_frame(ctx, ctx->stack)->dst;
    gc_barrier_val(ctx, ctx->stack, ctx->stack->vals.vals[dst] = val);
}
