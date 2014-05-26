vm_value vm_new_list();
void     vm_list_append(vm_value list, vm_value item);
size_t   vm_list_length(vm_value list);

vm_value vm_list_getitem(vm_value list, int index);
void     vm_list_setitem(vm_value list, int index, vm_value item);
