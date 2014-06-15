#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

void vm_call(vm_context* ctx, vm_value self, vm_value function, int argc, int base)
{
    vm_value interface;
    int lastbase;

    lastbase  = vm_stack_current_frame(ctx->stack)->base;
    interface = vm_get_interface(function);
    if (interface == vm_typespec_interface(&vm_apifunc_type))
    {

        vm_stack_c_push(ctx->stack, self, argc, lastbase+base);
        vm_stack_current_base(ctx->stack)[-1] = vm_null;
        vm_unbox_apifunc(function)(ctx);
        vm_stack_c_pop(ctx->stack);
    }
    else if (interface == vm_typespec_interface(&vm_closure_type))
    {
        vm_stack_push(ctx->stack, self, argc, lastbase+base);
    }
    else
    {
        VM_STUB();
    }
}

static vm_value vm_symbol_get = 0;
//static vm_value vm_symbol_set = 0;

void vm_getattr(vm_context* ctx, int base) //vm_value* base)
{
    vm_value interface, value, handler;
    vm_value subject = vm_stack_current_base(ctx->stack)[base+0];
    vm_value name    = vm_stack_current_base(ctx->stack)[base+1];

    interface = vm_get_interface(subject);
    if (vm_type_getitem(interface, name, &value))
    {
        interface = vm_get_interface(value);
        if (!vm_symbol_get) vm_symbol_get = vm_box_cstring("+get");
        if (interface == vm_typespec_interface(&vm_property_type))
        {
            handler = vm_property_get_get(value);
            if (handler == 0)
            {
                VM_STUB();
            }
            vm_stack_current_base(ctx->stack)[base+1] = subject;
            vm_call(ctx, subject, handler, 1, base+1);
            //VM_STUB(); //vm_call(ctx, subject, handler, base+1, 1);
        }
        else if (vm_type_getitem(interface, vm_symbol_get, &handler))
        {
            vm_stack_current_base(ctx->stack)[base+1] = subject;
            VM_STUB(); //vm_call(ctx, subject, handler, base+1, 1);
        }
        else
        {
            vm_stack_current_base(ctx->stack)[base+0] = value;
        }
    }
    else
    {
        VM_STUB();
    }
}

void vm_setattr(vm_context* ctx, vm_value* base)
{
    //vm_value subject = base[0];
    //vm_value name    = base[1];
    //vm_value value   = base[2];
    VM_STUB();
}

void vm_callattr(vm_context* ctx, vm_value subject, int argc, int base)
{
    vm_value interface, value;
    vm_value  name   = vm_stack_current_base(ctx->stack)[base+0];
    int       args   = base+1;
    interface = vm_get_interface(subject);

    if (vm_type_getitem(interface, name, &value))
    {
        vm_call(ctx, subject, value, argc-1, args);
        //VM_STUB(); //vm_call(ctx, subject, value, args, argc-1);
    }
    else
    {
        VM_STUB();
    }
}

void vm_loop(vm_context* ctx)
{
    if(ctx->stack == NULL) return;
    vm_frame* frame = vm_stack_current_frame(ctx->stack);
    uint8_t*  pc   = frame->pc;
    vm_value* base = vm_stack_current_base(ctx->stack);
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
        &&op_getattr,
        &&op_callattr,
        &&op_setattr,
        &&op_gt,
        &&op_sub,
        &&op_mul,
        &&op_loop,
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
    if (!vm_stack_return(ctx, base[a]))
    {
        return;
    }
    frame = vm_stack_current_frame(ctx->stack);
    base  = vm_stack_current_base(ctx->stack);
    pc    = frame->pc;
    goto next;
op_closure: /* closure dst func */
    base[a] = vm_box_object(
        vm_new_closure(vm_unbox_object(vm_list_getitem(frame->desc->functions, d)), frame->upvalues)
    );
    goto next;
op_call: /* call start stop */
    frame->pc   = pc;
    vm_call(ctx, frame->self, base[a], b-a-1, a+1);
    frame = vm_stack_current_frame(ctx->stack);
    base  = vm_stack_current_base(ctx->stack);
    pc    = frame->pc;
    goto next;
op_tailcall: /* tailcall start stop */
    /*
    ctx->frame--;
    c = -1;
    while (a < b) base[c++] = base[a++];
    vm_call(ctx, frame->self, base[-1], base, c);
    //vm_context_push(ctx, vm_unbox_object(base[-1]), base, c);
    ctx   = ctx->next;
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    */
    VM_STUB();
    goto next;
op_getattr: /* getattr base */
    frame->pc   = pc;

    vm_getattr(ctx, a);
    frame = vm_stack_current_frame(ctx->stack);
    base  = vm_stack_current_base(ctx->stack);
    pc    = frame->pc;
    goto next;
op_callattr: /* callattr subject start stop */
    frame->pc   = pc;
    vm_callattr(ctx, base[a], c - b, b);
    frame = vm_stack_current_frame(ctx->stack);
    base  = vm_stack_current_base(ctx->stack);
    pc    = frame->pc;
    /*
    frame->base = base;
    ctx   = ctx->next;
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    */
    goto next;
op_setattr: /* setattr base */
    VM_STUB();
    /*
    frame->pc   = pc;
    frame->base = base;
    vm_setattr(ctx, base+a);
    ctx   = ctx->next;
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    */
    goto next;
op_gt: /* gt dst src src */
    //base[a] = base[b] > base[c]
    if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
    {
        base[a] = vm_box_boolean(vm_unbox_double(base[b]) > vm_unbox_double(base[c]));
    } else
    {
        VM_STUB();
    }
    goto next;
op_sub: /* sub dst src src */
    if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
    {
        base[a] = vm_box_double(vm_unbox_double(base[b]) - vm_unbox_double(base[c]));
    } else
    {
        VM_STUB();
    }
    goto next;
op_mul: /* mul dst src src */
    if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
    {
        base[a] = vm_box_double(vm_unbox_double(base[b]) * vm_unbox_double(base[c]));
    } else
    {
        VM_STUB();
    }
    goto next;

op_loop: /* loop */
    goto next;
}
