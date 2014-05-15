// lousy man's garbage collector
#include <stdlib.h>
#include "value.h"
#include "gc.h"

typedef unsigned char uint8;

static int next = 0;
static uint8 pool[32 * 1024 * 1024];

void* gcAlloc(size_t size)
{
    void* addr;

    addr = pool+next;
    next += size;
    return addr;
}
