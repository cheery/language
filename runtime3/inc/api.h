#include <stdint.h>
#include "value.h"
#include "gc.h"

typedef struct vm_typespec vm_typespec;

#include "vm.h"
#include "context.h"

typedef struct
{
    const char* name;
    void (*func)(vm_context* ctx);
} vm_lib;

vm_value vm_new_api(vm_lib* lib);
vm_value vm_init();
size_t   vm_get_argc(vm_context* ctx);
vm_value  vm_get_value(vm_context* ctx, int index);
void* vm_get_self_object(vm_context* ctx, vm_typespec *spec);
void* vm_get_object(vm_context* ctx, int index, vm_typespec *spec);
void     vm_return_value(vm_context* ctx, vm_value value);

struct vm_typespec
{
    size_t   size;
    vm_value interface;
    vm_value (*factory)();
};

#include "list.h"
#include "dict.h"

void  vm_init_typespec(vm_typespec* spec);
void* vm_instantiate(vm_typespec* spec, size_t extra_size);
int   vm_has_interface(vm_value value, vm_typespec* spec);

vm_value vm_get_interface(vm_value value);
vm_value vm_typespec_interface(vm_typespec *spec);
void     vm_stub(const char* file, const char* function, int line) __attribute__((noreturn));

#define VM_STUB() vm_stub(__FILE__, __func__, __LINE__);

void vm_api_register(vm_value lib, const char* name, vm_value value);

vm_value vm_new_property(vm_value get, vm_value set, vm_value call);
vm_value vm_property_get_get(vm_value value);
vm_value vm_property_get_set(vm_value value);
vm_value vm_property_get_call(vm_value value);

// defined in src/type.c
extern vm_typespec vm_type;

vm_value vm_new_type();
void     vm_type_method(vm_value type, const char* name, vm_apifunc);

#define vm_type_getitem vm_dict_getitem
#define vm_type_setitem vm_dict_setitem

extern vm_typespec vm_string_type;
extern vm_typespec vm_apifunc_type;
extern vm_typespec vm_property_type;

#define vm_null (vm_box_object(0))
