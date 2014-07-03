#include "vm.h"

static void         vals_cover(vm_context *ctx, vm_stack *stack, int used);
static vm_frame*    push_frame(vm_context *ctx, vm_stack *stack);

vm_stack* vm_new_stack(vm_context *ctx, vm_val self, int argc, vm_val *argv)
{
    vm_stack *stack;
    vm_val   *base;
    int i;

    stack = gc_new(ctx, sizeof(vm_stack), vm_t_stack, interface_stub);
    stack->parent = NULL;
    stack->status = 0;
    stack->vals.used = 0;
    stack->vals.size = 0;
    stack->vals.vals = NULL;
    stack->frames.used = 0;
    stack->frames.size = 0;
    stack->frames.frames = NULL;
    stack->excepts.used = 0;
    stack->excepts.size = 0;
    stack->excepts.excepts = NULL;
    /*
     * while the stack is not running, the arguments are stored in the stack.
     */
    vals_cover(ctx, stack, argc+1);
    base = stack->vals.vals;
    base[0] = self;
    for (i = 0; i < argc; i++)
    {
        base[i+1] = argv[i];
    }
    return stack;
}

void vm_stack_push(vm_context *ctx, vm_stack *stack, vm_val self, int dst, int func, int argc)
{
    vm_builtin     *builtin;
    vm_closure     *clos;
    vm_descriptor  *desc;
    vm_frame       *frame;
    vm_object_type  type;
    int i, base;

    if(stack->frames.used > 0)
    {
        /* makes the func relative to the current base, if there is one.. makes things simpler. */
        base = vm_stack_current_frame(ctx, stack)->base;
        func += base;
        dst  += base;
    }
    
    if (ctx->stack != stack)
    {
        vm_panic(ctx);
    }

    type = vm_typeof(stack->vals.vals[func]);
    if (type == vm_t_builtin)
    {
        builtin = vm_unbox(ctx, stack->vals.vals[func], vm_t_builtin);
        frame   = push_frame(ctx, stack);
        /* add C stack frame, then set 'return value' null. */
        frame->clos = NULL;
        frame->pc   = argc;
        frame->dst  = dst;
        frame->base = func + 1;
        frame->top  = frame->base + argc;
        stack->vals.vals[frame->dst] = vm_null;
        builtin->func(ctx);
        /* check if stack frame is still a C stack frame, in that case remove it. */
        frame   = vm_stack_current_frame(ctx, stack);
        if (frame->clos == NULL)
        {
            stack->frames.used--;
        }
        /* fit the previous frame into the stack, if it was cut */
        frame   = vm_stack_current_frame(ctx, stack);
        vals_cover(ctx, stack, frame->top);
    }
    else if (type == vm_t_closure)
    {
        clos = vm_unbox(ctx, stack->vals.vals[func], vm_t_closure);
        desc = clos->desc;
        frame = push_frame(ctx, stack);
        gc_barrier(ctx, stack, frame->clos = clos);
        gc_barrier_val(ctx, stack, frame->self = self);
        frame->dst  = dst;
        frame->base = func + 1;
        frame->top  = frame->base + desc->valc;
        frame->pc   = 0;

        vals_cover(ctx, stack, frame->top);

        // create clean upvalues
        frame->upvalues = gc_realloc(ctx, NULL, sizeof(vm_upval*) * desc->nupvalc);
        for (i = 0; i < desc->nupvalc; i++)
        {
            gc_barrier(ctx, stack, frame->upvalues[i] = gc_new(ctx, sizeof(vm_upval), vm_t_upval, vm_null));
            frame->upvalues[i]->val = vm_null;
        }
        // fill unfilled values with null
        for(i = argc; i < desc->valc; i++)
        {
            vm_stack_current_base(ctx, stack)[i] = vm_null;
        }
    }
    else
    {
        vm_panic(ctx);
    }
}

int vm_stack_pop(vm_context *ctx, vm_stack *stack, vm_val retval)
{
    vm_frame *frame;
    size_t frame_id;

    frame = vm_stack_current_frame(ctx, stack);
    gc_barrier_val(ctx, stack, stack->vals.vals[frame->dst] = retval);

    frame_id = --stack->frames.used;
    // drop the exceptions of the frame
    while(stack->excepts.used > 0 && stack->excepts.excepts[stack->excepts.used-1].frame == frame_id)
    {
        stack->excepts.used--;
    }
    gc_realloc(ctx, frame->upvalues, 0);

    if (frame_id > 0)
    {
        frame = vm_stack_current_frame(ctx, stack);
        vals_cover(ctx, stack, frame->top);
        return 0;
    }
    else if (stack->parent == NULL && ctx->greenlet->parent == NULL)
    {
        return 1;
    }
    else
    {
        vm_panic(ctx);
        return 1;
    }

}

void vm_stack_resume(vm_context *ctx, vm_stack *stack, int argc, vm_val *argv)
{
    int i;

    if (stack->parent != NULL)
    {
        /* generator already executing */
        vm_panic(ctx);
    }
    if (stack->status == 2)
    {
        /* the stack is finished, raise stopiteration */
        vm_panic(ctx);
    }
    else if (stack->status == 1)
    {
        /* the stack was suspended */
        vm_panic(ctx);
    }
    else if (stack->status == 0)
    {
        /* boot up the stack, if impossible, raise an error */
        size_t  used;
        vm_val *base;


        used = stack->vals.used;
        vals_cover(ctx, stack, used + argc);
        base = stack->vals.vals;
        for (i = 0; i < argc; i++)
        {
            gc_barrier_val(ctx, stack, base[used+i] = argv[i]);
        }

        gc_barrier(ctx, stack, stack->parent = ctx->stack);
        ctx->stack = stack;
        vm_stack_push(ctx, stack, base[0], 0, 1, used+argc - 2);
    }
}

/*
//int vm_stack_resume(vm_context* ctx, vm_stack* stack, int argc, int base)
//{
//    int i;
//
//    if (stack->parent != NULL)
//    {
//        VM_STUB();
//    }
//
//    if (stack->top == -2)
//    {
//        return 0;
//    }
//    else if (stack->top == -1)
//    {
//        if (ctx->stack)
//        {
//            ctx->stack->ret = base - 1;
//        }
//        stack->parent = ctx->stack;
//        ctx->stack    = stack;
//        for (i = 0; i < argc; i++)
//        {
//            stack->values[stack->frames[0].top++] = stack->parent->values[base+i];
//        }
//        vm_stack_push(stack, stack->frames[0].self, stack->frames[0].top-1, 1);
//        return 1;
//    }
//    else if (stack->ret == -1)
//    {
//        VM_STUB();
//    }
//    else
//    {
//        ctx->stack->ret = base - 1;
//        stack->parent = ctx->stack;
//        ctx->stack    = stack;
//        if (argc > 0)
//        {
//            stack->values[stack->ret] = stack->parent->values[base+0];
//        }
//        else
//        {
//            stack->values[stack->ret] = vm_null;
//        }
//        stack->ret = -1;
//        return 1;
//    }
//}
//
//void vm_stack_c_push(vm_stack* stack, vm_value self, int argc, int base)
//{
//    //vm_frame*   lastframe;
//    vm_frame*   frame;
//
//    //lastframe = vm_stack_current_frame(stack);
//    frame = &stack->frames[++stack->top];
//    frame->desc = NULL;
//    frame->pc   = NULL;
//    frame->base = |*lastframe->base +*| base;
//    frame->top  = |*lastframe->base +*| base + argc;
//    frame->self = self;
//}
//
//void vm_stack_c_pop(vm_stack* stack)
//{
//    stack->top--;
//}
//
//int vm_stack_return(vm_context* ctx, vm_value retval)
//{
//    vm_stack* stack;
//
//    stack = ctx->stack;
//    while(stack->exc >= 0 && stack->excepts[stack->exc].frame == stack->top)
//    {
//        stack->exc--;
//    }
//    if (stack->top == 0)
//    {
//        stack->top = -2;
//        if (stack->parent)
//        {
//            ctx->stack = stack->parent;
//            ctx->stack->values[ctx->stack->ret] = retval;
//            return 1;
//        }
//        else if (ctx->greenlet->parent)
//        {
//            ctx->greenlet = ctx->greenlet->parent;
//            ctx->stack    = ctx->greenlet->stack;
//            stack = ctx->stack;
//            vm_stack_current_base(stack)[-1] = retval;
//            stack->top--;
//            return 1;
//        }
//        else
//        {
//            return 0;
//        }
//    }
//    else
//    {
//        vm_stack_current_base(stack)[-1] = retval;
//        stack->top--;
//        return 1;
//    }
//}
//
//int vm_stack_resume_greenlet(vm_context* ctx, vm_greenlet* greenlet)
//{
//    vm_stack* stack;
//    int argc, i;
//
//    stack = greenlet->stack;
//    argc  = vm_get_argc(ctx);
//    if (stack->top == -1)
//    {
//        for (i = 0; i < argc; i++)
//        {
//            stack->values[stack->frames[0].top++] = vm_get_value(ctx, i);
//        }
//        vm_stack_push(stack, stack->frames[0].self, stack->frames[0].top-1, 1);
//        stack->top++; // cancel vm_stack_c_pop
//    } else if (stack->top != 2)
//    {
//        vm_stack_current_base(stack)[-1] = vm_get_value(ctx, 0);
//    } else
//    {
//        VM_STUB();
//    }
//
//    ctx->greenlet->stack = ctx->stack;
//    ctx->greenlet = greenlet;
//    ctx->stack    = stack;
//    return 1;
//}
//
//void vm_stack_suspend(vm_context* ctx, int argc, int base)
//{
//    vm_stack* stack;
//    
//    stack = ctx->stack;
//    stack->ret = base - 1;
//
//    ctx->stack = stack->parent;
//    if (argc > 0)
//    {
//        ctx->stack->values[ctx->stack->ret] = stack->values[base+0];
//    }
//    else
//    {
//        ctx->stack->values[ctx->stack->ret] = vm_null;
//    }
//    stack->parent = NULL;
//}
//
//int       vm_stack_dead(vm_stack* stack)
//{
//    return stack->top == -2;
//}
//
//
//void      vm_stack_except(vm_stack* stack, uint8_t* pc, int type, int base)
//{
//    vm_except* except;
//
//    except = &stack->excepts[++stack->exc];
//    except->frame = stack->top;
//    except->type = type;
//    except->base = base;
//    except->pc = pc;
//}
//
//
//int       vm_stack_except_type(vm_context* ctx)
//{
//    vm_stack* stack;
//
//    stack = ctx->stack;
//    do
//    {
//        if (stack->exc >= 0)
//        {
//            return stack->excepts[stack->exc].type;
//        }
//        stack = stack->parent;
//    } while(stack);
//    return VM_APESHIT;
//}
//
//int       vm_stack_except_jump(vm_context* ctx)
//{
//    vm_stack* stack;
//    vm_except* except;
//
//    stack = ctx->stack;
//    do
//    {
//        if (stack->exc >= 0)
//        {
//            ctx->stack = stack;
//            except = &stack->excepts[stack->exc--];
//            stack->top = except->frame;
//            stack->frames[stack->top].pc = except->pc;
//            return except->base;
//        }
//        stack->parent = NULL;
//        stack->top = -2;
//        stack = stack->parent;
//    } while(stack);
//    VM_STUB();
//}
//
//void      vm_stack_except_discard(vm_context* ctx)
//{
//    vm_stack* stack;
//
//    stack = ctx->stack;
//    do
//    {
//        if (stack->exc >= 0)
//        {
//            ctx->stack = stack;
//            stack->exc--;
//            return;
//        }
//        stack = stack->parent;
//    } while(stack);
//    VM_STUB();
//}
//*/
//
//

vm_except* vm_stack_push_except(vm_context *ctx, vm_stack *stack)
{
    size_t used;
    vm_except_array *array;
    vm_except       *except;

    array = &stack->excepts;
    used = ++array->used;
    if (used >= array->size)
    {
        array->size  = used*2 + 32;
        array->excepts = gc_realloc(ctx, array->excepts, sizeof(vm_except) * array->size);
    }
    except = &array->excepts[used - 1];
    except->frame = stack->frames.used - 1;
    return except;
}

void vm_stack_pop_except(vm_context *ctx, vm_stack *stack)
{
    vm_except_array *array = &stack->excepts;
    if (array->used - 1 < 0)
    {
        vm_panic(ctx);
    }
    if (array->excepts[array->used - 1].frame != stack->frames.used - 1)
    {
        vm_panic(ctx);
    }
    array->used -= 1;
}

vm_except* vm_stack_current_except(vm_context *ctx, vm_stack *stack)
{
    return &stack->excepts.excepts[stack->excepts.used-1];
}

vm_frame* vm_stack_current_frame(vm_context *ctx, vm_stack *stack)
{
    if(stack->frames.used == 0)
    {
        vm_panic(ctx);
    }
    return &stack->frames.frames[stack->frames.used - 1];
}

vm_val* vm_stack_current_base(vm_context *ctx, vm_stack *stack)
{
    vm_frame *frame;

    frame = vm_stack_current_frame(ctx, stack);
    return stack->vals.vals + frame->base;
}

int  vm_stack_pre_call(vm_context *ctx, vm_stack *stack, int count)
{
    vm_frame *frame;

    if(stack->frames.used > 0)
    {
        /* makes the func relative to the current base, if there is one.. makes things simpler. */
        frame = vm_stack_current_frame(ctx, stack);
        count += frame->top;
        vals_cover(ctx, stack, count);
        return frame->top;
    }
    else
    {
        vals_cover(ctx, stack, count);
        return 0;
    }
}

static void vals_cover(vm_context *ctx, vm_stack *stack, int used)
{
    vm_val_array *array = &stack->vals;

    if (used >= array->size)
    {
        array->size  = used*2 + 32;
        array->vals = gc_realloc(ctx, array->vals, sizeof(vm_val) * array->size);
    }
    array->used  = used;
}

static vm_frame* push_frame(vm_context *ctx, vm_stack *stack)
{
    size_t used = stack->frames.used + 1;
    vm_frame_array *array = &stack->frames;

    if (used >= array->size)
    {
        array->size  = used*2 + 32;
        array->frames = gc_realloc(ctx, array->frames, sizeof(vm_frame) * array->size);
    }
    array->used = used;
    return &array->frames[used-1];
}
