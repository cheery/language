
vm_list* vm_new_list(vm_context *ctx, vm_val interface);
void vm_list_append(vm_context *ctx, vm_list* list, vm_val item);
size_t vm_list_length(vm_context *ctx, vm_list* list);
vm_val vm_list_getitem(vm_context *ctx, vm_list* list, int index);
void vm_list_setitem(vm_context *ctx, vm_list* list, int index, vm_val item);
