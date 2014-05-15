#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "value.h"
#include "gc.h"

Value boxSysFunc(Value (*func)(long argc, Value* argv))
{
    struct system_func* sf;
    sf = gcAlloc(sizeof(*sf));
    sf->object.type = T_SYSTEM_FUNCTION;
    sf->call = func;
    return boxObject(sf);
}

Value sysPrint(long argc, Value* argv)
{
    int i;

    for(i = 0; i < argc; i++)
    {
        if (isString(argv[i]))
        {
            printf("%s", unboxString(argv[i]));
        } else if (isFixnum(argv[i]))
        {
            printf("%li", unboxFixnum(argv[i]));
        } else
        {
            printf("<print unimplemented>");
        }
    }
    printf("\n");
    return boxObject(0);
}

Value sysInput(long argc, Value* argv)
{
    size_t length = 0;
    char*  string = NULL;
    Value  retval;

    if ((argc == 1) && isString(argv[0]))
    {
        printf("%s", unboxString(argv[0]));
    }
    if (getline(&string, &length, stdin) == 0)
    {
        length = 1;
        string = "\n";
    }
    length = strnlen(string, length);
    retval = boxString(length-1, string);
    free(string);
    return retval;
}
