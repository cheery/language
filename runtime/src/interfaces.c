//size_t   vm_dict_sizeof()
//{
//    return sizeof(vm_dict);
//}
//
//static vm_value dict_factory()
//{
//    
//    return vm_box_object(0);
//}
//vm_typespec vm_dict_type = {sizeof(vm_dict), 0, dict_factory};
//

//vm_value vm_dict_init(void* address)
//{
//    vm_dict *dict;
//
//    dict = address;
//    dict->size   = 0;
//    dict->length = 0;
//    dict->entries = NULL;
//    return vm_box_object(dict);
//}
//static vm_value stack_factory()
//{
//    vm_value type;
//    
//    type = vm_new_type();
//    return type;
//}
//
//vm_typespec vm_stack_type = {sizeof(vm_stack), 0, stack_factory};

//
//static vm_value descriptor_factory()
//{
//    vm_value type;
//    
//    type = vm_new_type();
//    return type;
//}
//
//vm_typespec vm_descriptor_type = {sizeof(vm_descriptor), 0, descriptor_factory};
//
//static vm_value closure_factory()
//{
//    vm_value type;
//    
//    type = vm_new_type();
//    return type;
//}
//
//vm_typespec vm_closure_type = {sizeof(vm_closure), 0, closure_factory};
//
//static void append(vm_context* ctx)
//{
//    vm_list* list;
//
//    list = vm_get_self_object(ctx, &vm_list_type);
//    vm_list_append(vm_box_object(list), vm_get_value(ctx, 0));
//    vm_return_value(ctx, vm_box_object(0));
//}
//
//static void get_length(vm_context* ctx)
//{
//    vm_list* list;
//
//    list = vm_get_object(ctx, 0, &vm_list_type);
//    vm_return_value(ctx, vm_box_integer(list->length));
//}
//
//static void getitem(vm_context* ctx)
//{
//    vm_list* list;
//    long index;
//
//    list  = vm_get_self_object(ctx, &vm_list_type);
//    index = vm_get_integer(ctx, 0);
//    if (index >= list->length)
//    {
//        VM_STUB();
//    }
//    vm_return_value(ctx, list->values[index]);
//}
//
//
//static vm_value list_factory()
//{
//    vm_value type;
//    
//    type = vm_new_type();
//    vm_type_method(type, "append", append);
//    vm_type_method(type, "+getitem", getitem);
//    vm_type_setitem(type, vm_box_cstring("length"), vm_new_property(vm_box_apifunc(get_length), 0, 0));
//
//    return type;
//}
//vm_typespec vm_list_type = {sizeof(vm_list), 0, list_factory};
//
//
//static void greenlet_switch(vm_context* ctx)
//{
//    vm_greenlet* next;
//
//    next = vm_get_self_object(ctx, &vm_greenlet_type);
//
//    while(next && vm_stack_dead(next->stack))
//    {
//        next = next->parent;
//    }
//
//    if (next == NULL)
//    {
//        VM_STUB();
//    }
//
//    vm_stack_resume_greenlet(ctx, next);
//}
//
//static void greenlet_dead(vm_context* ctx)
//{
//    vm_greenlet* greenlet;
//
//    greenlet = vm_get_self_object(ctx, &vm_greenlet_type);
//    vm_return_value(ctx, vm_box_boolean(vm_stack_dead(greenlet->stack)));
//}
//
//static void greenlet_parent(vm_context* ctx)
//{
//    vm_greenlet* greenlet;
//
//    greenlet = vm_get_self_object(ctx, &vm_greenlet_type);
//    vm_return_value(ctx, vm_box_object(greenlet->parent));
//}
//
//static vm_value greenlet_factory()
//{
//    vm_value type;
//    
//    type = vm_new_type();
//    vm_type_method(type, "switch", greenlet_switch);
//    vm_type_setitem(type, vm_box_cstring("dead"),   vm_new_property(vm_box_apifunc(greenlet_dead),   0, 0));
//    vm_type_setitem(type, vm_box_cstring("parent"), vm_new_property(vm_box_apifunc(greenlet_parent), 0, 0));
//    return type;
//}
//
//vm_typespec vm_greenlet_type = {sizeof(vm_greenlet), 0, greenlet_factory};
//
//
//
//#include <stdlib.h>
//#include "api.h"
//
//typedef struct 
//{
//    vm_object object;
//    vm_value  string;
//    size_t    index;
//} vm_string_iterator;
//
//static vm_value string_iterator_factory();
//vm_typespec vm_string_iterator_type = {sizeof(vm_string_iterator), 0, string_iterator_factory};
//
//static void string_iterator_next(vm_context* ctx)
//{
//    vm_string_iterator* iterator;
//    char* string;
//    size_t length;
//
//    iterator = vm_get_self_object(ctx, &vm_string_iterator_type);
//    length = vm_string_length(iterator->string);
//    string = vm_unbox_string(iterator->string);
//    if (iterator->index < length)
//    {
//        vm_return_value(ctx, vm_box_string(1, &string[iterator->index++]));
//    }
//    else
//    {
//        vm_stopiteration(ctx);
//    }
//}
//
//static vm_value string_iterator_factory()
//{
//    vm_value type;
//
//    type = vm_new_type();
//    vm_type_method(type, "next", string_iterator_next);
//    return type;
//}
//
//static void new_iterator(vm_context* ctx)
//{
//    vm_string_iterator* iterator;
//
//    iterator = vm_instantiate(&vm_string_iterator_type, 0);
//    vm_get_self_object(ctx, &vm_string_type);
//    iterator->string = vm_get_self(ctx);
//    iterator->index  = 0;
//    vm_return_value(ctx, vm_box_object(iterator));
//}
//
//static vm_value string_factory()
//{
//    vm_value type;
//
//    type = vm_new_type();
//    vm_type_method(type, "+iter", new_iterator);
//    return type;
//}
//
//vm_typespec vm_string_type = {0, 0, string_factory};
//
//#include <stdlib.h>
//#include "api.h"
//
//static vm_value type_factory()
//{
//    vm_value basetype;
//    vm_object* dict;
//
//    vm_type.size = vm_dict_sizeof();
//    dict = malloc(vm_type.size);
//    basetype = vm_dict_init(dict);
//    dict->interface = basetype;
//
//    return basetype;
//}
//
//vm_typespec vm_type = {0, 0, type_factory};
//
//vm_value vm_new_type()
//{
//    vm_object* dict;
//
//    dict = vm_instantiate(&vm_type, 0);
//    return vm_dict_init(dict);
//}
//
//void vm_type_method(vm_value type, const char* name, vm_apifunc func)
//{
//    vm_type_setitem(type, vm_box_cstring(name), vm_box_apifunc(func));
//}
