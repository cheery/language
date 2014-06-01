extern vm_typespec vm_dict_type;

size_t   vm_dict_sizeof();
vm_value vm_dict_init(void* dict);
vm_value vm_new_dict();

size_t   vm_dict_length(vm_value dict);
int      vm_dict_getitem(vm_value dict, vm_value key, vm_value* item);
void     vm_dict_setitem(vm_value dict, vm_value key, vm_value item);
