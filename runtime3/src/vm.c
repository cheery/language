#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gc.h"
#include "value.h"
#include "list.h"
#include "context.h"

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

    desc = malloc(sizeof(*desc));
    desc->argc = argc;
    desc->valc = valc;
    desc->upvalc = upvalc;
    desc->upcopyc = upcopyc;
    desc->upcopy = malloc(sizeof(*upcopy) * upcopyc);
    memcpy(desc->upcopy, upcopy, sizeof(*upcopy) * upcopyc);
    desc->size = size;
    desc->program = malloc(size);
    memcpy(desc->program, program, size);
    desc->functions = functions;
    desc->constants = constants;
    return desc;
}

vm_closure* vm_new_closure(vm_descriptor* desc, vm_value** upvalues)
{
    vm_closure* closure;
    int i;

    closure = malloc(sizeof(*closure) + sizeof(*upvalues) * desc->upcopyc);
    closure->desc = desc;
    for(i = 0; i < desc->upcopyc; i++)
    {
        closure->upvalues[i] = upvalues[desc->upcopy[i]];
    }
    return closure;
}

vm_context* vm_new_context()
{
    vm_context* context;

    context = malloc(sizeof(*context));
    context->frame = -1;
    return context;
}

void vm_context_push(vm_context* context, vm_closure* closure, vm_value* base, int argc)
{
    vm_descriptor* desc = closure->desc;
    int i;

    if (base == NULL) base = context->stack + 1;

    vm_frame* frame = &context->frames[++context->frame];
    frame->desc    = desc;
    frame->closure = closure;
    frame->pc      = desc->program;
    frame->base    = base;
    frame->top     = base + desc->valc;
    frame->upvalues = malloc(sizeof(*frame->upvalues) * desc->upvalc);
    for(i = 0; i < desc->upcopyc; i++)
    {
        frame->upvalues[i] = closure->upvalues[i];
    }
    frame->upvalues[i] = malloc(sizeof(vm_value) * (desc->upvalc - i));
    for(i++; i < desc->upvalc; i++)
    {
        frame->upvalues[i] = frame->upvalues[i-1] + 1;
    }
    for(i = argc; i < desc->argc; i++)
    {
        frame->base[i] = vm_box_object(NULL);
    }
}

void vm_call(vm_context* ctx, vm_value function, vm_value* base, int argc)
{
    if (vm_unbox_tag(function) == vm_tag_apifunc)
    {
        ctx->cbase = base;
        ctx->ctop  = base+argc;
        base[-1] = vm_box_object(NULL);
        vm_unbox_apifunc(function)(ctx);
    }
    else
    {
        vm_context_push(ctx, vm_unbox_object(function), base, argc);
    }
    //vm_context_push(ctx, vm_unbox_object(base[a]), base+a+1, b-a-1);
}

void vm_loop(vm_context* ctx)
{
    if(ctx->frame < 0) return;
    vm_frame* frame = &ctx->frames[ctx->frame];
    uint8_t*  pc   = frame->pc;
    vm_value* base = frame->base;
    int op, a, b, c, d;

    const void* optable[] = {
        &&op_move,
        &&op_loadbool,
        &&op_loadnull,
        &&op_getupval, // 3
        &&op_setupval,
        &&op_getconst,
        &&op_setconst,
        &&op_jump,     // 7
        &&op_test,
        &&op_testn,    // 9
        &&op_return,
        &&op_closure,
        &&op_call,     // 12
        &&op_tailcall,
    };

        //op_getattr,
        //op_setattr,
        //op_callattr,
        //op_getitem,
        //op_setitem,
        //add sub mul div pow unm not cat
        //ne eq lt le gt ge
next:
    op = pc[0];
    a  = pc[1];
    b  = pc[2];
    c  = pc[3];
    d  = b | c << 8;
    pc += 4;
    printf("%i: op[%i](%i, %i, %i | %i)\n", (int)((pc - frame->desc->program - 4) >> 2), op, a, b, c, d);
    goto *optable[op];
op_move: /* move dst src */
    base[a] = base[b];
    goto next;
op_loadbool: /* loadbool dst imm */
    base[a] = vm_box_boolean(b);
    goto next;
op_loadnull: /* loadnull start stop */
    while (a < b) base[a++] = vm_box_object(NULL);
    goto next;
op_getupval: /* getupval dst upval */
    base[a] = *frame->upvalues[d];
    goto next;
op_setupval: /* setupval src upval */
    *frame->upvalues[d] = base[a];
    goto next;
op_getconst: /* getconst dst const */
    base[a] = vm_list_getitem(frame->desc->constants, d);
    goto next;
op_setconst: /* setconst src const */
    vm_list_setitem(frame->desc->constants, d, base[a]);
    goto next;
op_jump: /* jump ign off */
    pc += d - 0x8000;
    goto next;
op_test: /* test src off */
    if (vm_true(base[a])) pc += d - 0x8000;
    goto next;
op_testn: /* testn src off */
    if (vm_false(base[a])) pc += d - 0x8000;
    goto next;
op_return: /* return src */
    base[-1] = base[a];
    if(--ctx->frame < 0) return;
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_closure: /* closure dst func */
    base[a] = vm_box_object(
        vm_new_closure(vm_unbox_object(vm_list_getitem(frame->desc->functions, d)), frame->upvalues)
    );
    goto next;
op_call: /* call start stop */
    frame->pc   = pc;
    frame->base = base;
    vm_call(ctx, base[a], base+a+1, b-a-1);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_tailcall: /* tailcall start, stop */
    ctx->frame--;
    c = -1;
    while (a < b) base[c++] = base[a++];
    vm_call(ctx, base[-1], base, c);
    //vm_context_push(ctx, vm_unbox_object(base[-1]), base, c);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
}
