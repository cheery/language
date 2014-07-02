vm_dict* vm_new_dict(vm_context *ctx, vm_val interface);
size_t vm_dict_length(vm_context *ctx, vm_dict *dict);
int vm_dict_getitem(vm_context *ctx, vm_dict *dict, vm_val key, vm_val *item);
void vm_dict_setitem(vm_context *ctx, vm_dict *dict, vm_val key, vm_val item);
