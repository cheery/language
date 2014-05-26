#include <stdint.h>
#include "gc.h"
#include "value.h"
#include "context.h"
#include "list.h"
#include "dict.h"

typedef struct
{
    const char* name;
    void (*func)(vm_context* ctx);
} vm_lib;

vm_value vm_new_api(vm_lib* lib);
vm_value vm_init();
size_t   vm_get_argc(vm_context* ctx);
vm_value vm_get_value(vm_context* ctx, int index);
void     vm_return_value(vm_context* ctx, vm_value value);
