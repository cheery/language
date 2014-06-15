#include <stdlib.h>
#include "api.h"

struct vm_stack
{
    vm_object object;
    vm_stack* parent;
    int       top;
    int       ret;
    vm_value  values[4096];
    vm_frame  frames[1024];
};

static vm_value stack_factory()
{
    vm_value type;
    
    type = vm_new_type();
    return type;
}

vm_typespec vm_stack_type = {sizeof(vm_stack), 0, stack_factory};

vm_stack* vm_new_stack(vm_value self, int argc, vm_value* argv)
{
    vm_stack* stack;
    int i;

    stack = vm_instantiate(&vm_stack_type, 0);
    stack->top     = -1;
    stack->ret     = -1;
    stack->parent  = NULL;
    stack->frames[0].self = self;
    stack->frames[0].base = 0;
    stack->frames[0].top  = 0;
    for (i = 0; i < argc; i++)
    {
        stack->values[stack->frames[0].top++] = argv[i];
    }

    return stack;
}

void vm_stack_push(vm_stack* stack, vm_value self, int argc, int base)
{
    vm_closure* closure;
    vm_descriptor* desc;
    //vm_frame*   lastframe;
    vm_frame*   frame;
    int i;

    if (vm_get_interface(stack->values[base-1]) != vm_typespec_interface(&vm_closure_type))
    {
        VM_STUB();
    }
    closure = vm_unbox_object(stack->values[base-1]);
    desc    = closure->desc;

    //lastframe = vm_stack_current_frame(stack);
    frame = &stack->frames[++stack->top];
    frame->desc = desc;
    frame->pc   = desc->program;
    frame->base = /*lastframe->base +*/ base;
    frame->top  = /*lastframe->base +*/ base + desc->valc;
    frame->self = self;

    // configure upvalues
    frame->upvalues = realloc(NULL, sizeof(*frame->upvalues) * desc->upvalc);
    for(i = 0; i < desc->upcopyc; i++)
    {
        frame->upvalues[i] = closure->upvalues[i];
    }
    frame->upvalues[i] = realloc(NULL, sizeof(vm_value) * (desc->upvalc - i));
    for(i++; i < desc->upvalc; i++)
    {
        frame->upvalues[i] = frame->upvalues[i-1] + 1;
    }

    // fill unfilled values with null
    for(i = argc; i < desc->argc; i++)
    {
        stack->values[frame->base+i] = vm_null;
    }
}

void vm_stack_c_push(vm_stack* stack, vm_value self, int argc, int base)
{
    //vm_frame*   lastframe;
    vm_frame*   frame;

    //lastframe = vm_stack_current_frame(stack);
    frame = &stack->frames[++stack->top];
    frame->desc = NULL;
    frame->pc   = NULL;
    frame->base = /*lastframe->base +*/ base;
    frame->top  = /*lastframe->base +*/ base + argc;
    frame->self = self;
}

void vm_stack_c_pop(vm_stack* stack)
{
    stack->top--;
}

int vm_stack_return(vm_context* ctx, vm_value retval)
{
    vm_stack* stack;

    stack = ctx->stack;
    if (stack->top == 0)
    {
        stack->top = -2;
        if (stack->parent)
        {
            ctx->stack = stack->parent;
            ctx->stack->values[ctx->stack->ret] = retval;
            return 1;
        }
        else if (ctx->greenlet->parent)
        {
            ctx->greenlet = ctx->greenlet->parent;
            ctx->stack    = ctx->greenlet->stack;
            stack = ctx->stack;
            vm_stack_current_base(stack)[-1] = retval;
            stack->top--;
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        vm_stack_current_base(stack)[-1] = retval;
        stack->top--;
        return 1;
    }
}

int vm_stack_resume(vm_context* ctx, vm_stack* stack, int argc, int base)
{
    int i;

    if (stack->parent != NULL)
    {
        VM_STUB();
    }

    if (stack->top == -2)
    {
        return 0;
    }
    else if (stack->top == -1)
    {
        if (ctx->stack)
        {
            ctx->stack->ret = base - 1;
        }
        stack->parent = ctx->stack;
        ctx->stack    = stack;
        for (i = 0; i < argc; i++)
        {
            stack->values[stack->frames[0].top++] = stack->parent->values[base+i];
        }
        vm_stack_push(stack, stack->frames[0].self, stack->frames[0].top-1, 1);
        return 1;
    }
    else if (stack->ret == -1)
    {
        VM_STUB();
    }
    else
    {
        ctx->stack->ret = base - 1;
        stack->parent = ctx->stack;
        ctx->stack    = stack;
        if (argc > 0)
        {
            stack->values[stack->ret] = stack->parent->values[base+0];
        }
        else
        {
            stack->values[stack->ret] = vm_null;
        }
        stack->ret = -1;
        return 1;
    }
}

int vm_stack_resume_greenlet(vm_context* ctx, vm_greenlet* greenlet)
{
    vm_stack* stack;
    int argc, i;

    stack = greenlet->stack;
    argc  = vm_get_argc(ctx);
    if (stack->top == -1)
    {
        for (i = 0; i < argc; i++)
        {
            stack->values[stack->frames[0].top++] = vm_get_value(ctx, i);
        }
        vm_stack_push(stack, stack->frames[0].self, stack->frames[0].top-1, 1);
        stack->top++; // cancel vm_stack_c_pop
    } else if (stack->top != 2)
    {
        vm_stack_current_base(stack)[-1] = vm_get_value(ctx, 0);
    } else
    {
        VM_STUB();
    }

    ctx->greenlet->stack = ctx->stack;
    ctx->greenlet = greenlet;
    ctx->stack    = stack;
    return 1;
}

void vm_stack_suspend(vm_context* ctx, int argc, int base)
{
    vm_stack* stack;
    
    stack = ctx->stack;
    stack->ret = base - 1;

    ctx->stack = stack->parent;
    if (argc > 0)
    {
        ctx->stack->values[ctx->stack->ret] = stack->values[base+0];
    }
    else
    {
        ctx->stack->values[ctx->stack->ret] = vm_null;
    }
    stack->parent = NULL;
}

int       vm_stack_dead(vm_stack* stack)
{
    return stack->top == -2;
}

vm_frame* vm_stack_current_frame(vm_stack* stack)
{
    return &stack->frames[stack->top];
}

vm_value* vm_stack_current_base(vm_stack* stack)
{
    return &stack->values[vm_stack_current_frame(stack)->base];
}
