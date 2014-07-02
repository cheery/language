#include "vm.h"
#include <string.h>

vm_path* vm_path_parse(vm_context *ctx, const char* cpath)
{
    size_t len = strlen(cpath);
    int i = 0, start = 0;
    vm_path *path = NULL;

    if (cpath[0] == '/')
    {
        path = vm_new_path(ctx, NULL, NULL);
        i     = 1;
        start = 1;
    }

    for (; i < len; i++)
    {
        if (cpath[i] == '/')
        {
            path  = vm_new_path(ctx, vm_new_string(ctx, i-start, cpath+start), path);
            start = i+1;
        }
    }
    if (start < len)
    {
        path = vm_new_path(ctx, vm_new_string(ctx, len-start, cpath+start), path);
    }

    return path;
}

vm_path* vm_new_path(vm_context *ctx, vm_string *string, vm_path *parent)
{
    vm_path *path;

    path = gc_new(ctx, sizeof(vm_path), vm_t_path, interface_stub);
    path->string = string;
    path->parent = parent;
    return path;
}

vm_string* vm_path_cpath(vm_context *ctx, vm_path *path)
{
    vm_path *current;
    size_t length, index, l;

    current = path;
    length  = 0;
    while (current != NULL)
    {
        if (current->string)
        {
            length += current->string->length;
        }
        current = current->parent;
        if (current)
        {
            length++;
        }
    }

    char cpath[length];

    current = path;
    index   = length;
    while (current != NULL)
    {
        if (current->string)
        {
            l = current->string->length;
            index -= l;
            memcpy(cpath+index, current->string->data, l);
        }
        current = current->parent;
        if (current)
        {
            index--;
            cpath[index] = '/';
        }
    }
    return vm_new_string(ctx, length, cpath);
}
