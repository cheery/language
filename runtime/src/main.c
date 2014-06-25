#include <setjmp.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include "api.h"

static char* ambrosia_path;

char* path_join(const char* directory, const char* name);
vm_value  open_module(const char* path);

int main(int argc, char* argv[])
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
