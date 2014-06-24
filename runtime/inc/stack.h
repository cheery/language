typedef struct vm_frame vm_frame;
typedef struct vm_stack vm_stack;
typedef struct vm_greenlet vm_greenlet;

extern vm_typespec vm_stack_type;

struct vm_frame
{
    vm_descriptor* desc;
    vm_value **upvalues;
    uint8_t* pc;
    int      base;
    int      top;
    vm_value self;
};

#define VM_APESHIT          0
#define VM_NORMAL_EXCEPTION 1
#define VM_HAPPY_ITERATOR   2


vm_stack* vm_new_stack(vm_value self, int argc, vm_value* argv);
void      vm_stack_push(vm_stack* stack, vm_value self, int argc, int base);
void      vm_stack_c_push(vm_stack* stack, vm_value self, int argc, int base);
void      vm_stack_c_pop(vm_stack* stack);

int       vm_stack_return(vm_context* ctx, vm_value retval);
int       vm_stack_resume(vm_context* ctx, vm_stack* stack, int argc, int base);
int       vm_stack_resume_greenlet(vm_context* ctx, vm_greenlet* greenlet);

void      vm_stack_suspend(vm_context* ctx, int argc, int base);
int       vm_stack_dead(vm_stack* stack);
vm_frame* vm_stack_current_frame(vm_stack* stack);

vm_value* vm_stack_current_base(vm_stack* stack);
void      vm_stack_except(vm_stack* stack, uint8_t* pc, int type, int base);
void      vm_stack_except_drop(vm_stack* stack, int count);

int       vm_stack_except_type(vm_context* ctx);
int       vm_stack_except_jump(vm_context* ctx);
void      vm_stack_except_discard(vm_context* ctx);
