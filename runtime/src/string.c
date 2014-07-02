#include "vm.h"
#include <string.h>

vm_string* vm_new_cstring(vm_context *ctx, const char* string)
{
    return vm_new_string(ctx, strlen(string), string);
}

vm_string* vm_new_string(vm_context *ctx, size_t length, const char* data)
{
    vm_string* string;

    string = gc_new(ctx, sizeof(vm_string) + length + 1, vm_t_string, interface_stub);
    string->length = length;
    memcpy(string->data, data, length);
    string->data[length] = 0;
    return string;
}

vm_arraybuffer* vm_new_arraybuffer(vm_context *ctx, size_t length, const uint8_t* data)
{
    vm_arraybuffer* buffer;
   
    buffer = gc_new(ctx, sizeof(vm_arraybuffer) + length, vm_t_arraybuffer, interface_stub);
    buffer->length = length;
    if (data)
    {
        memcpy(buffer->data, data, length);
    }
    return buffer;
}
