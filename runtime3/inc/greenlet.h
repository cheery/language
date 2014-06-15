typedef struct vm_greenlet vm_greenlet;

extern vm_typespec vm_greenlet_type;

struct vm_greenlet
{
    vm_object object;
    vm_greenlet* parent;
    vm_stack* stack;
};

vm_greenlet* vm_new_greenlet(vm_greenlet* parent, vm_value self, int argc, vm_value* argv);
