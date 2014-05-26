typedef uint64_t vm_value;

typedef enum {
    vm_tag_object = 0,
    vm_tag_constant,
    vm_tag_string,
    vm_tag_apifunc,
    vm_tag_double = 8
} vm_tag;

vm_tag   vm_unbox_tag(vm_value value);
int      vm_true(vm_value value);
int      vm_false(vm_value value);

vm_value vm_box_object(void* object);
void*    vm_unbox_object(vm_value value);
vm_value vm_box_boolean(int boolean);

int      vm_unbox_boolean(vm_value value);
vm_value vm_box_double(double value);
double   vm_unbox_double(vm_value value);

vm_value vm_box_cstring(const char* string);
vm_value vm_box_string(size_t length, const char* string);
char*    vm_unbox_string(vm_value string);
size_t   vm_string_length(vm_value string);

typedef struct vm_context vm_context;
typedef void (*vm_apifunc)(vm_context* ctx);
vm_value   vm_box_apifunc(vm_apifunc func);
vm_apifunc vm_unbox_apifunc(vm_value func);
