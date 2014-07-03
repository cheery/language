#include "vm.h"
#include <stdio.h>
/*
#include <stdlib.h>
#include <string.h>
#include "api.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


static void input(vm_context* ctx)
{
    vm_value value;

    if (vm_get_argc(ctx) == 1)
    {
        value = vm_get_value(ctx, 0);
        printf("%s", vm_unbox_string(value));
    }

    size_t length = 0;
    char*  string = NULL;
    vm_value retval;

    if (getline(&string, &length, stdin) == 0)
    {
        length = 1;
        string = "\n";
    }
    length = strnlen(string, length);
    retval = vm_box_string(length-1, string);
    free(string);

    vm_return_value(ctx, retval);
}

static void make_list(vm_context* ctx)
{
    vm_return_value(ctx, vm_new_list());
}

static void getcurrent(vm_context* ctx)
{
    vm_return_value(ctx, vm_box_object(ctx->greenlet));
}

static void greenlet(vm_context* ctx)
{
    vm_greenlet* greenlet;
    vm_value  self;
    vm_value* args;
    int argc;

    argc = vm_get_argc(ctx);
    self = vm_stack_current_frame(ctx->stack)->self;
    args = vm_stack_current_base(ctx->stack);
    greenlet = vm_new_greenlet(ctx->greenlet, self, argc, args);
    vm_return_value(ctx, vm_box_object(greenlet));
}

static void throw_exception(vm_context* ctx)
{
    ctx->exception_type  = vm_null;
    ctx->exception_value = vm_null;
    longjmp(ctx->exception_return, 1);
}

static void read_file(vm_context* ctx)
{
    vm_value path;
    struct stat stat;
    int fd;

    path = vm_get_value(ctx, 0);
    if (vm_unbox_tag(path) != vm_tag_string)
    {
        VM_STUB();
    }

    fd = open(vm_unbox_string(path), O_RDONLY);
    if (fd < 0)
    {
        VM_STUB();
    }
    fstat(fd, &stat);
    char string[stat.st_size];
    if (stat.st_size != read(fd, string, stat.st_size))
    {
        VM_STUB();
    }
    close(fd);
    vm_return_value(ctx, vm_box_string(stat.st_size, string));
}

static void ord(vm_context* ctx)
{
    vm_value obj;

    obj = vm_get_value(ctx, 0);
    if (vm_unbox_tag(obj) != vm_tag_string)
    {
        VM_STUB();
    }
    vm_return_value(ctx, vm_box_integer(vm_unbox_string(obj)[0]));
}

static vm_lib vm_base_library[] = {
    {"print", print},
    {"input", input},
    {"list", make_list},
    {"getcurrent", getcurrent},
    {"greenlet",   greenlet},
    {"error",      throw_exception},
    {"read_file",  read_file},
    {"ord",        ord},
    {NULL, NULL}
};

static void print(vm_context* ctx)
{
    size_t argc = vm_get_argc(ctx);
    int j, i;
    vm_value value;

    for(j = 0; j < argc; j++)
    {
        value = vm_get_value(ctx, j);
        switch (vm_unbox_tag(value))
        {
            case vm_tag_string:
                printf("%s", vm_unbox_string(value));
                break;
            case vm_tag_object:
                {
                    char template[] = "<xxxx-xxxx-xxxx>";
                    uint64_t halfbytes = (uint64_t)value;
                    for (i = 11; i >= 0; i--)
                    {
                         
                        template[1 + i + i / 4] = "0123456789abcdef"[halfbytes & 15];
                        halfbytes >>= 4;
                    }
                    printf("%s", template);
                }
                break;
            case vm_tag_double:
                printf("%g", vm_unbox_double(value));
                break;
            case vm_tag_constant:
                if (value == vm_box_boolean(1))
                {
                    printf("true");
                    break;
                }
                if (value == vm_box_boolean(0))
                {
                    printf("false");
                    break;
                }
            default:
                {
                    char template[] = "<xxxx-xxxx-xxxx-xxxx>";
                    uint64_t halfbytes = (uint64_t)value;
                    for (i = 15; i >= 0; i--)
                    {
                         
                        template[1 + i + i / 4] = "0123456789abcdef"[halfbytes & 15];
                        halfbytes >>= 4;
                    }
                    printf("%s", template);
                }
        }
    }
    printf("\n");
}
*/

static void print(vm_context* ctx)
{
    size_t argc;
   
    argc = vm_c_argc(ctx);
    int i, j;
    vm_val value;
    vm_string *string;

    for (j = 0; j < argc; j++)
    {
        value = vm_c_load(ctx, j);
        switch (vm_typeof(value))
        {
            case vm_t_string:
                string = vm_unbox(ctx, value, vm_t_string);
                printf("%s", string->data);
                break;
            case vm_t_object:
                {
                    char template[] = "<xxxx-xxxx-xxxx>";
                    uint64_t halfbytes = (uint64_t)value;
                    for (i = 11; i >= 0; i--)
                    {
                         
                        template[1 + i + i / 4] = "0123456789abcdef"[halfbytes & 15];
                        halfbytes >>= 4;
                    }
                    printf("%s", template);
                }
                break;
            case vm_t_integer:
                printf("%li", vm_unbox_integer(ctx, value));
                break;
            case vm_t_double:
                printf("%g", vm_unbox_double(ctx, value));
                break;
            case vm_t_constant:
                if (vm_unbox_bool(ctx, value))
                {
                    printf("true");
                }
                else
                {
                    printf("false");
                }
                break;
           default:
           {
                char template[] = "<xxxx-xxxx-xxxx-xxxx>";
                uint64_t halfbytes = (uint64_t)value;
                for (i = 15; i >= 0; i--)
                {
                    template[1 + i + i / 4] = "0123456789abcdef"[halfbytes & 15];
                    halfbytes >>= 4;
                }
                printf("%s", template);
           }
       }
   }
   printf("\n");
}

vm_dict* vm_load_baselib(vm_context *ctx)
{
    vm_dict *lib;

    lib = vm_new_dict(ctx, interface_stub);

    vm_dict_setitem(ctx, lib,
            vm_box(ctx, vm_new_cstring(ctx, "print")),
            vm_box(ctx, vm_new_builtin(ctx, "print", print)));

    /*
    vm_value lib;
    vm_value iter_func;

    lib = vm_new_api(vm_base_library);
    //vm_api_register(lib, "list", vm_typespec_interface(&vm_list_type));
    //vm_api_register(lib, "dict", vm_typespec_interface(&vm_dict_type));

    uint8_t program[] = {
        22, 0, 0, 0, // ITER 0, 0
        10, 0, 0, 0, // RETURN 0
    };
    vm_descriptor* desc = vm_new_descriptor(
            1, 1, 0, 0, sizeof(program), NULL, program, vm_null, vm_null);
    iter_func = vm_box_object(vm_new_closure(desc, NULL));

    vm_api_register(lib, "iter", iter_func);
    */
    return lib;
}

