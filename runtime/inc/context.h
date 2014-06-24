#include <setjmp.h>

struct vm_context
{
    //vm_object object;
    //vm_value* cbase;
    //vm_value* ctop;
    //vm_value  cself;
    //vm_context*  next;
    vm_stack*    stack;
    vm_greenlet* greenlet;

    jmp_buf          exception_return;
    vm_value         exception_type;
    vm_value         exception_value;
    //vm_descriptor* exception_desc;
    //size_t         exception_pc;
};

// extern vm_typespec vm_context_type;

vm_context* vm_new_context();
void vm_context_bootup(vm_context* context, vm_closure* closure);

// defined in src/vm.c
void vm_call(vm_context* ctx, vm_value self, vm_value function, int base, int argc);
