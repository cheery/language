#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

static vm_value descriptor_factory()
{
    return vm_box_object(0);
}

vm_typespec vm_descriptor_type = {sizeof(vm_descriptor), 0, descriptor_factory};

static vm_value closure_factory()
{
    return vm_box_object(0);
}

vm_typespec vm_closure_type = {sizeof(vm_closure), 0, closure_factory};

static vm_value context_factory()
{
    return vm_box_object(0);
}

vm_typespec vm_context_type = {sizeof(vm_context), 0, context_factory};

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

    desc = vm_instantiate(&vm_descriptor_type, 0);
    desc->argc = argc;
    desc->valc = valc;
    desc->upvalc = upvalc;
    desc->upcopyc = upcopyc;
    desc->upcopy = realloc(NULL, sizeof(*upcopy) * upcopyc);
    memcpy(desc->upcopy, upcopy, sizeof(*upcopy) * upcopyc);
    desc->size = size;
    desc->program = realloc(NULL, size);
    memcpy(desc->program, program, size);
    desc->functions = functions;
    desc->constants = constants;
    return desc;
}

vm_closure* vm_new_closure(vm_descriptor* desc, vm_value** upvalues)
{
    vm_closure* closure;
    int i;

    closure = vm_instantiate(&vm_closure_type, sizeof(*upvalues) * desc->upcopyc);
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

    context = vm_instantiate(&vm_context_type, 0);
    context->frame = -1;
    return context;
}

void vm_context_push(vm_context* context, vm_value self, vm_closure* closure, vm_value* base, int argc)
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
    frame->self    = self;
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
    for(i = argc; i < desc->argc; i++)
    {
        frame->base[i] = vm_box_object(NULL);
    }
}

void vm_call(vm_context* ctx, vm_value self, vm_value function, vm_value* base, int argc)
{
    vm_value interface;

    interface = vm_get_interface(function);

    if (interface == vm_typespec_interface(&vm_apifunc_type))
    {
        ctx->cself = self;
        ctx->cbase = base;
        ctx->ctop  = base+argc;
        base[-1] = vm_box_object(NULL);
        vm_unbox_apifunc(function)(ctx);
    }
    else if (interface == vm_typespec_interface(&vm_descriptor_type))
    {
        vm_context_push(ctx, self, vm_unbox_object(function), base, argc);
    }
    else
    {
        VM_STUB();
    }
}

static vm_value vm_symbol_get = 0;
static vm_value vm_symbol_set = 0;

void vm_getattr(vm_context* ctx, vm_value* base)
{
    vm_value interface, value, handler;
    vm_value subject = base[0];
    vm_value name    = base[1];

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
            base[1] = subject;
            vm_call(ctx, subject, handler, base+1, 1);
        }
        else if (vm_type_getitem(interface, vm_symbol_get, &handler))
        {
            base[1] = subject;
            vm_call(ctx, subject, handler, base+1, 1);
        }
        else
        {
            base[0] = value;
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

void vm_callattr(vm_context* ctx, vm_value subject, vm_value* base, int argc)
{
    vm_value interface, value;
    vm_value  name   = base[0];
    vm_value* args   = base+1;
    interface = vm_get_interface(subject);
    if (vm_type_getitem(interface, name, &value))
    {
        vm_call(ctx, subject, value, args, argc-1);
    }
    else
    {
        VM_STUB();
    }
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
        &&op_getattr,
        &&op_callattr,
        &&op_setattr
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
    vm_call(ctx, frame->self, base[a], base+a+1, b-a-1);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_tailcall: /* tailcall start stop */
    ctx->frame--;
    c = -1;
    while (a < b) base[c++] = base[a++];
    vm_call(ctx, frame->self, base[-1], base, c);
    //vm_context_push(ctx, vm_unbox_object(base[-1]), base, c);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_getattr: /* getattr base */
    frame->pc   = pc;
    frame->base = base;
    vm_getattr(ctx, base+a);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_callattr: /* callattr subject start stop */
    frame->pc   = pc;
    frame->base = base;
    vm_callattr(ctx, base[a], base+b, c - b);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
op_setattr: /* setattr base */
    frame->pc   = pc;
    frame->base = base;
    vm_setattr(ctx, base+a);
    frame = &ctx->frames[ctx->frame];
    pc    = frame->pc;
    base  = frame->base;
    goto next;
}
