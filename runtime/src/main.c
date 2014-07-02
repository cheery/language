#include "vm.h"
#include <stdio.h>
#include <error.h>

int main(int argc, char* argv[])
{
    vm_context _ctx;
    vm_context *ctx = &_ctx;

    ctx->greenlet = NULL;
    ctx->stack    = NULL;

    vm_path    *ambrosia_path;
    const char *cpath;

    cpath = getenv("AMBROSIA_PATH");
    if (cpath == NULL)
    {
        error(1, 0, "please set AMBROSIA_PATH, I'm not doing guesswork yet.");
    }
    ambrosia_path = vm_path_parse(ctx, cpath);

    printf("AMBROSIA_PATH=%s\n", vm_path_cpath(ctx, ambrosia_path)->data);
    printf("index path: %s\n",
            vm_path_cpath(
                ctx,
                vm_new_path(
                    ctx,
                    vm_new_cstring(ctx, "index.o"),
                    ambrosia_path))->data);

    vm_dict *baselib, *namespace;

    baselib   = vm_new_dict(ctx, interface_stub);
    namespace = vm_new_dict(ctx, interface_stub);

    vm_descriptor *desc;

    desc = vm_module_load(
            ctx,
            vm_new_path(ctx, vm_new_cstring(ctx, "index.o"), ambrosia_path),
            baselib,
            namespace);

    //
    // create simple closure, load it later.
//    vm_arraybuffer *upcopy  = vm_new_arraybuffer(ctx, 0, NULL);
//    vm_arraybuffer *program = vm_new_arraybuffer(ctx, 8, NULL);
//    vm_list     *functions = NULL;
//    vm_list     *constants = NULL;
//    desc = vm_new_descriptor(
//            ctx,
//            0, /* flags */
//            0, /* argc  */
//            0, /* valc  */
//            0, /* nupvalc */
//            upcopy,
//            program,
//            functions,
//            constants);

    vm_val args[8];
    args[0] = vm_box(ctx, vm_new_closure(ctx, desc));
    ctx->greenlet = vm_new_greenlet(ctx, NULL, vm_null, 1, args);
    printf("ambrosia interpreter\n");

    // start the interpreter
    // get the interpreter rolling, sketch things out along.
    vm_stack_resume(ctx, ctx->greenlet->stack, 0, NULL);
    vm_loop(ctx);


    return EXIT_SUCCESS;
}


/*
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include "api.h"

static char* ambrosia_path;

char* path_join(const char* directory, const char* name);
vm_value  open_module(const char* path);

{
    char* bootfile;

    printf("hello\n");

    ambrosia_path = getenv("AMBROSIA_PATH");
    if (ambrosia_path == NULL)
    {
        error(1, 0, "please set AMBROSIA_PATH, I'm not doing guesswork yet.");
    }
    bootfile = path_join(ambrosia_path, "index.o");

    printf("ambrosia path = '%s'\n", ambrosia_path);
    printf("bootfile = '%s'\n", bootfile);
    open_module(bootfile);
    free(bootfile);

    return EXIT_SUCCESS;
}

char* path_join(const char* directory, const char* name)
{
    char* out;
    size_t a_len = strlen(directory);
    size_t b_len = strlen(name);
    size_t i;
    if (a_len <= 0) error(1, 0, "no directory path");

    if (directory[a_len-1] == '/') a_len -= 1;

    out = malloc(a_len+b_len+2);
    for (i = 0; i < a_len; i++) out[i] = directory[i];
    out[a_len] = '/';
    for (i = 0; i < b_len; i++) out[a_len+1+i] = name[i];
    out[a_len+b_len+1] = 0;
    return out;
}
*/
