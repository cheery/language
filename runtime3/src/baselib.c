#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "api.h"

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

static vm_lib vm_base_library[] = {
    {"print", print},
    {"input", input},
    {"list", make_list},
    {NULL, NULL}
};

vm_value vm_load_baselib()
{
    vm_value lib;

    lib = vm_new_api(vm_base_library);
    //vm_api_register(lib, "list", vm_typespec_interface(&vm_list_type));
    //vm_api_register(lib, "dict", vm_typespec_interface(&vm_dict_type));

    return lib;
}
