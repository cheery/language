void    gc_barrier(vm_context *ctx, void *subject, void *object);
void    gc_barrier_val(vm_context *ctx, void *subject, vm_val val);
void*   gc_new(vm_context *ctx, size_t size, vm_object_type type, vm_val interface);
void*   gc_realloc(vm_context *ctx, void *address, size_t size);
