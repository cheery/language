#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include "objects.h"
#include "closure.h"
#include "dict.h"
#include "gc.h"
#include "greenlet.h"
#include "list.h"
#include "stack.h"

// baselib.c
vm_dict* vm_load_baselib(vm_context *ctx);

// builtin.c
vm_builtin* vm_new_builtin(vm_context* ctx, const char* name, vm_builtin_func func);

size_t vm_c_argc(vm_context *ctx);
vm_val vm_c_load(vm_context *ctx, size_t index);
void   vm_c_return(vm_context *ctx, vm_val val);

// cmp.c
int vm_cmp(vm_context *ctx, vm_val a, vm_val b);

// string.c
vm_string* vm_new_cstring(vm_context *ctx, const char* string);
vm_string* vm_new_string(vm_context *ctx, size_t length, const char* data);
vm_arraybuffer* vm_new_arraybuffer(vm_context *ctx, size_t length, const uint8_t* data);
#define vm_arraybuffer_count(a, format) ((a)->length / sizeof(format))

#define interface_stub vm_null
// interfaces.c
vm_val     vm_interface_method(vm_context *ctx, vm_val subject, vm_val name);


vm_path*   vm_path_parse(vm_context *ctx, const char* cpath);
vm_path*   vm_new_path(vm_context *ctx, vm_string *string, vm_path *parent);
vm_string* vm_path_cpath(vm_context *ctx, vm_path *path);

// module.c
vm_descriptor* vm_module_load(vm_context *ctx, vm_path *path, vm_dict *baselib, vm_dict *names);

/* priority, reason: cannot evaluate anything without */
/* module.c */
/* compiler2.py */
/* add the function flags */

/* liveless without */
/* stack.h +c */
/* vm.h +c */

/* context.h +c unnecessary? */
/* closure.c grep vm_panic, new_closure halfimplemented */

/* interface, grep gc_alloc later, and put all in place. */
/* also interface_stub -mark used */

/* remaining: */

/* api.h +c */
/* baselib.c */
/* type.c  belongs to the interface sooner/later*/

/* INVARIANT: 
 *      the objects that are created together before the next
 *      gc_cycle can be linked together without a gc_barrier call.
 */

/* INVARIANT:
 *      moving stuff around in same object doesn't require a gc_barrier.
 *      setting stuff true/false/null doesn't require a gc_barrier.
 */


void vm_loop(vm_context* ctx);
