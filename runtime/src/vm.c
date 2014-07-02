#include "vm.h"
#include <stdio.h>

void vm_loop(vm_context* ctx)
{
    if (ctx->stack == NULL || ctx->greenlet == NULL)
    {
        vm_panic(ctx);
    }

    const void* optable[] = {
        &&op_add,
        &&op_and,
        &&op_booland,
        &&op_boolnot,
        &&op_boolor,
        &&op_call,
        &&op_callvarg,
        &&op_closure,
        &&op_div,
        &&op_eq,
        &&op_except,
        &&op_finally,
        &&op_floordiv,
        &&op_ge,
        &&op_getattr,
        &&op_getconst,
        &&op_getitem,
        &&op_getupval,
        &&op_gt,
        &&op_jump,
        &&op_le,
        &&op_loadfalse,
        &&op_loadnull,
        &&op_loadtrue,
        &&op_lshift,
        &&op_lt,
        &&op_mod,
        &&op_move,
        &&op_mul,
        &&op_ne,
        &&op_neg,
        &&op_next,
        &&op_not,
        &&op_or,
        &&op_pos,
        &&op_raise,
        &&op_reraise,
        &&op_return,
        &&op_rshift,
        &&op_self,
        &&op_setattr,
        &&op_setconst,
        &&op_setitem,
        &&op_setupval,
        &&op_sub,
        &&op_tailcall,
        &&op_tailcallvarg,
        &&op_test,
        &&op_testnot,
        &&op_xor
    };
    vm_frame *frame = vm_stack_current_frame(ctx, ctx->stack);
    vm_val   *base  = vm_stack_current_base(ctx, ctx->stack);
    uint8_t  *pc    = frame->pc + frame->clos->desc->program->data;
    int op, a, b, c, d, exc_type, exc_base;
next:
    op = pc[0];
    a  = pc[1];
    b  = pc[2];
    c  = pc[3];
    d  = b | c << 8;
    pc += 4;
    fprintf(stderr, "%i: op[%i](%i, %i, %i | %i)\n", (int)((pc - frame->clos->desc->program->data - 4) >> 2), op, a, b, c, d);
    goto *optable[op];
op_add:
op_and:
op_booland:
op_boolnot:
op_boolor:
op_call:
op_callvarg:
op_closure:
op_div:
op_eq:
op_except:
op_finally:
op_floordiv:
op_ge:
op_getattr:
op_getconst:
op_getitem:
op_getupval:
op_gt:
    goto notimplemented;
op_jump: /* jump drop off */
    if (a > 0)
    {
        vm_panic(ctx);
        //vm_stack_except_drop(ctx, ctx->stack, a);
    }
    pc += d - 0x8000;
    goto next;
op_le:
    goto notimplemented;
op_loadfalse: /* loadfalse start stop */
    while (a < b) base[a++] = vm_box_bool(ctx, 0);
    goto next;
op_loadnull: /* loadnull start stop */
    while (a < b) base[a++] = vm_null;
    goto next;
op_loadtrue: /* loadtrue start stop */
    while (a < b) base[a++] = vm_box_bool(ctx, 1);
    goto next;
op_lshift:
op_lt:
op_mod:
    goto notimplemented;
op_move: /* move dst src */
    base[a] = base[b];
    goto next;
op_mul:
op_ne:
op_neg:
op_next:
op_not:
op_or:
op_pos:
op_raise:
op_reraise:
op_return:
op_rshift:
op_self:
op_setattr:
op_setconst:
op_setitem:
op_setupval:
op_sub:
op_tailcall:
op_tailcallvarg:
op_test:
op_testnot:
op_xor:
    goto notimplemented;
notimplemented:
    vm_panic(ctx);
    goto next;
}


// void vm_call(vm_context* ctx, vm_value self, vm_value function, int argc, int base)
// {
//     vm_value interface;
//     int lastbase;
// 
//     lastbase  = vm_stack_current_frame(ctx->stack)->base;
//     interface = vm_get_interface(function);
//     if (interface == vm_typespec_interface(&vm_apifunc_type))
//     {
// 
//         vm_stack_c_push(ctx->stack, self, argc, lastbase+base);
//         vm_stack_current_base(ctx->stack)[-1] = vm_null;
//         vm_unbox_apifunc(function)(ctx);
//         vm_stack_c_pop(ctx->stack);
//     }
//     else if (interface == vm_typespec_interface(&vm_closure_type))
//     {
//         vm_stack_push(ctx->stack, self, argc, lastbase+base);
//     }
//     else
//     {
//         VM_STUB();
//     }
// }
// 
// static vm_value vm_symbol_get = 0;
// //static vm_value vm_symbol_set = 0;
// static vm_value vm_symbol_iter = 0;
// static vm_value vm_symbol_next = 0;
// static vm_value vm_symbol_getitem = 0;
// 
// void vm_getattr(vm_context* ctx, int base) //vm_value* base)
// {
//     vm_value interface, value, handler;
//     vm_value subject = vm_stack_current_base(ctx->stack)[base+0];
//     vm_value name    = vm_stack_current_base(ctx->stack)[base+1];
// 
//     interface = vm_get_interface(subject);
//     if (vm_type_getitem(interface, name, &value))
//     {
//         interface = vm_get_interface(value);
//         if (!vm_symbol_get) vm_symbol_get = vm_box_cstring("+get");
//         if (interface == vm_typespec_interface(&vm_property_type))
//         {
//             handler = vm_property_get_get(value);
//             if (handler == 0)
//             {
//                 VM_STUB();
//             }
//             vm_stack_current_base(ctx->stack)[base+1] = subject;
//             vm_call(ctx, subject, handler, 1, base+1);
//             //VM_STUB(); //vm_call(ctx, subject, handler, base+1, 1);
//         }
//         else if (vm_type_getitem(interface, vm_symbol_get, &handler))
//         {
//             vm_stack_current_base(ctx->stack)[base+1] = subject;
//             VM_STUB(); //vm_call(ctx, subject, handler, base+1, 1);
//         }
//         else
//         {
//             vm_stack_current_base(ctx->stack)[base+0] = value;
//         }
//     }
//     else
//     {
//         VM_STUB();
//     }
// }
// 
// void vm_setattr(vm_context* ctx, vm_value* base)
// {
//     //vm_value subject = base[0];
//     //vm_value name    = base[1];
//     //vm_value value   = base[2];
//     VM_STUB();
// }
// 
// void vm_iter(vm_context* ctx, int base, int src)
// {
//     vm_value interface, handler;
//     vm_value self    = vm_stack_current_frame(ctx->stack)->self;
//     vm_value subject = vm_stack_current_base(ctx->stack)[src];
// 
//     interface = vm_get_interface(subject);
// 
//     if (!vm_symbol_iter) vm_symbol_iter = vm_box_cstring("+iter");
//     if (vm_type_getitem(interface, vm_symbol_iter, &handler))
//     {
//         vm_stack_current_base(ctx->stack)[base+0] = handler;
//         vm_stack_current_base(ctx->stack)[base+1] = self;
//         vm_call(ctx, subject, handler, 1, base+1);
//     }
//     else
//     {
//         vm_stack_current_base(ctx->stack)[base] = subject;
//     }
// }
// 
// void vm_next(vm_context* ctx, int base, int iter, uint8_t* pc)
// {
//     vm_value interface, handler;
//     vm_value iterator = vm_stack_current_base(ctx->stack)[iter];
// 
//     vm_stack_except(ctx->stack, pc, VM_HAPPY_ITERATOR, 0);
//     interface = vm_get_interface(iterator);
//     if (!vm_symbol_next) vm_symbol_next = vm_box_cstring("next");
//     if (vm_type_getitem(interface, vm_symbol_next, &handler))
//     {
//         vm_stack_current_base(ctx->stack)[base+0] = handler;
//         vm_call(ctx, iterator, handler, 0, base+1);
//     }
//     else
//     {
//         VM_STUB();
//     }
// }
// 
// void vm_callattr(vm_context* ctx, vm_value subject, int argc, int base)
// {
//     vm_value interface, value;
//     vm_value  name   = vm_stack_current_base(ctx->stack)[base+0];
//     int       args   = base+1;
//     interface = vm_get_interface(subject);
// 
//     if (vm_type_getitem(interface, name, &value))
//     {
//         vm_call(ctx, subject, value, argc-1, args);
//         //VM_STUB(); //vm_call(ctx, subject, value, args, argc-1);
//     }
//     else
//     {
//         VM_STUB();
//     }
// }
// 
// void vm_getitem(vm_context* ctx, int base)
// {
//     vm_value interface, handler;
//     vm_value subject = vm_stack_current_base(ctx->stack)[base];
// 
//     interface = vm_get_interface(subject);
//     if (!vm_symbol_getitem) vm_symbol_getitem = vm_box_cstring("+getitem");
//     if (vm_type_getitem(interface, vm_symbol_getitem, &handler))
//     {
//         vm_stack_current_base(ctx->stack)[base+0] = handler;
//         vm_call(ctx, subject, handler, 1, base+1);
//     }
//     else
//     {
//         VM_STUB();
//     }
// }
// 
// {
//     if(ctx->stack == NULL) return;
//     vm_frame* frame = vm_stack_current_frame(ctx->stack);
//     uint8_t*  pc   = frame->pc;
//     vm_value* base = vm_stack_current_base(ctx->stack);
//     int op, a, b, c, d, e;
//     int except_type, except_base;
// 
//     const void* uptable[] = {
//         &&up_move,
//         &&up_loadbool,
//         &&up_loadnull,
//         &&up_getupval, // 3
//         &&up_setupval,
//         &&up_getconst,
//         &&up_setconst,
//         &&up_jump,     // 7
//         &&up_test,
//         &&up_testn,    // 9
//         &&up_return,   // 10
//         &&up_closure,
//         &&up_call,     // 12
//         &&up_tailcall,
//         &&up_getattr,
//         &&up_callattr,
//         &&up_setattr,  // 16
//         &&up_gt,
//         &&up_sub,
//         &&up_mul,
//         &&up_loup,
//         &&up_except,
//         &&up_iter,     // 22
//         &&up_next,
//         &&up_nextc,
//         &&up_ge,
//         &&up_eq,
//         &&up_getitem,
//     };
// 
//     if ((e = setjmp(ctx->exception_return)))
//     {
//         except_type = vm_stack_except_type(ctx);
//         if (except_type == VM_APESHIT)
//         {
//             VM_STUB();
//         }
//         else if (except_type == VM_NORMAL_EXCEPTION)
//         {
//             //while(vm_stack_unroll(ctx));
//             except_base = vm_stack_except_jump(ctx);
//         }
//         else if (except_type == VM_HAPPY_ITERATOR)
//         {
//             if (ctx->exception_type != vm_typespec_interface(&vm_stupiteration_type))
//             {
//                 vm_stack_except_discard(ctx);
//                 longjmp(ctx->exception_return, e);
//             }
//             except_base = vm_stack_except_jump(ctx);
//         }
//         else
//         {
//             VM_STUB();
//         }
//         frame = vm_stack_current_frame(ctx->stack);
//         pc    = frame->pc;
//         base  = vm_stack_current_base(ctx->stack);
//     }
//         //up_getitem,
//         //up_setitem,
//         //add sub mul div pow unm not cat
//         //ne eq lt le gt ge
// next:
//     up = pc[0];
//     a  = pc[1];
//     b  = pc[2];
//     c  = pc[3];
//     d  = b | c << 8;
//     pc += 4;
//     fprintf(stderr, "%i: up[%i](%i, %i, %i | %i)\n", (int)((pc - frame->desc->program - 4) >> 2), up, a, b, c, d);
//     goto *uptable[up];
// up_move: /* move dst src */
//     base[a] = base[b];
//     goto next;
// up_loadbool: /* loadbool dst imm */
//     base[a] = vm_box_boolean(b);
//     goto next;
// up_loadnull: /* loadnull start stup */
//     while (a < b) base[a++] = vm_box_object(NULL);
//     goto next;
// up_getupval: /* getupval dst upval */
//     base[a] = *frame->upvalues[d];
//     goto next;
// up_setupval: /* setupval src upval */
//     *frame->upvalues[d] = base[a];
//     goto next;
// up_getconst: /* getconst dst const */
//     base[a] = vm_list_getitem(frame->desc->constants, d);
//     goto next;
// up_setconst: /* setconst src const */
//     vm_list_setitem(frame->desc->constants, d, base[a]);
//     goto next;
// up_jump: /* jump drup off */
//     if (a > 0) vm_stack_except_drup(ctx->stack, a);
//     pc += d - 0x8000;
//     goto next;
// up_test: /* test src off */
//     if (vm_true(base[a])) pc += d - 0x8000;
//     goto next;
// up_testn: /* testn src off */
//     if (vm_false(base[a])) pc += d - 0x8000;
//     goto next;
// up_return: /* return src */
//     if (!vm_stack_return(ctx, base[a]))
//     {
//         return;
//     }
//     frame = vm_stack_current_frame(ctx->stack);
//     base  = vm_stack_current_base(ctx->stack);
//     pc    = frame->pc;
//     goto next;
// up_closure: /* closure dst func */
//     base[a] = vm_box_object(
//         vm_new_closure(vm_unbox_object(vm_list_getitem(frame->desc->functions, d)), frame->upvalues)
//     );
//     goto next;
// up_call: /* call start stup */
//     frame->pc   = pc;
//     vm_call(ctx, frame->self, base[a], b-a-1, a+1);
//     frame = vm_stack_current_frame(ctx->stack);
//     base  = vm_stack_current_base(ctx->stack);
//     pc    = frame->pc;
//     goto next;
// up_tailcall: /* tailcall start stup */
//     /*
//     ctx->frame--;
//     c = -1;
//     while (a < b) base[c++] = base[a++];
//     vm_call(ctx, frame->self, base[-1], base, c);
//     //vm_context_push(ctx, vm_unbox_object(base[-1]), base, c);
//     ctx   = ctx->next;
//     frame = &ctx->frames[ctx->frame];
//     pc    = frame->pc;
//     base  = frame->base;
//     */
//     VM_STUB();
//     goto next;
// up_getattr: /* getattr base */
//     frame->pc   = pc;
// 
//     vm_getattr(ctx, a);
//     frame = vm_stack_current_frame(ctx->stack);
//     base  = vm_stack_current_base(ctx->stack);
//     pc    = frame->pc;
//     goto next;
// up_callattr: /* callattr subject start stup */
//     frame->pc   = pc;
//     vm_callattr(ctx, base[a], c - b, b);
//     frame = vm_stack_current_frame(ctx->stack);
//     base  = vm_stack_current_base(ctx->stack);
//     pc    = frame->pc;
//     /*
//     frame->base = base;
//     ctx   = ctx->next;
//     frame = &ctx->frames[ctx->frame];
//     pc    = frame->pc;
//     base  = frame->base;
//     */
//     goto next;
// up_setattr: /* setattr base */
//     VM_STUB();
//     /*
//     frame->pc   = pc;
//     frame->base = base;
//     vm_setattr(ctx, base+a);
//     ctx   = ctx->next;
//     frame = &ctx->frames[ctx->frame];
//     pc    = frame->pc;
//     base  = frame->base;
//     */
//     goto next;
// up_eq: /* eq dst src src */
//     //base[a] = base[b] > base[c]
//     if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
//     {
//         base[a] = vm_box_boolean(vm_unbox_double(base[b]) == vm_unbox_double(base[c]));
//     } else
//     {
//         VM_STUB();
//     }
//     goto next;
// up_gt: /* gt dst src src */
//     //base[a] = base[b] > base[c]
//     if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
//     {
//         base[a] = vm_box_boolean(vm_unbox_double(base[b]) > vm_unbox_double(base[c]));
//     } else
//     {
//         VM_STUB();
//     }
//     goto next;
// up_ge: /* ge dst src src */
//     //base[a] = base[b] > base[c]
//     if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
//     {
//         base[a] = vm_box_boolean(vm_unbox_double(base[b]) >= vm_unbox_double(base[c]));
//     } else
//     {
//         VM_STUB();
//     }
//     goto next;
// up_sub: /* sub dst src src */
//     if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
//     {
//         base[a] = vm_box_double(vm_unbox_double(base[b]) - vm_unbox_double(base[c]));
//     } else
//     {
//         VM_STUB();
//     }
//     goto next;
// up_mul: /* mul dst src src */
//     if (vm_unbox_tag(base[b]) == vm_unbox_tag(base[c]) && vm_tag_double == vm_unbox_tag(base[c]))
//     {
//         base[a] = vm_box_double(vm_unbox_double(base[b]) * vm_unbox_double(base[c]));
//     } else
//     {
//         VM_STUB();
//     }
//     goto next;
// 
// up_loup: /* loup */
//     goto next;
// 
// up_except: /* expect base off */
//     vm_stack_except(ctx->stack, pc + d - 0x8000, VM_NORMAL_EXCEPTION, a);
//     goto next;
// 
// up_iter: /* iter base src */
//     vm_iter(ctx, a, b);
//     goto next;
// 
// up_next: /* next base iter */
//     d  = pc[2] | pc[3] << 8;
//     pc += 4;
//     vm_next(ctx, a, b, pc + d - 0x8000);
//     goto next;
// 
// up_nextc: /* nextc ign off */
//     VM_STUB();
//     goto next;
// 
// up_getitem: /* getitem base */
//     vm_getitem(ctx, a);
//     goto next;
// }
