#include <stdio.h>
#include <string.h>
#include "value.h"
#include "gc.h"

#define NAN_MASK 0xFFF8000000000000L
#define DAT_MASK 0x0000FFFFFFFFFFFFL
#define FIXNUM_SIGN     0x0000800000000000L
#define FIXNUM_SIGN_EXT 0xFFFF800000000000L
#define DATA(x) (x & DAT_MASK)
#define TAG(x) ((x) >> 48)
#define TAGGED(x, v) ((long)(x) << 48 | (long)(v) & DAT_MASK)

#define True  (TAGGED(2, 1))
#define False (TAGGED(2, 0))

struct string
{
    size_t length;
    char   string[];
};

int isNull(Value value)    { return value == 0L; }
int isBoolean(Value value) { return (value == True) || (value == False); }
int isTrue(Value value)    { return value == True;  }
int isFalse(Value value)   { return value == False; }
int isObject(Value value) { return TAG(value) == 0; }
int isFixnum(Value value) { return TAG(value) == 1; }
int isString(Value value) { return TAG(value) == 2; }
int isDouble(Value value) { return (value & NAN_MASK != 0); }

Value boxNull() { return 0L; }
Value boxObject(void* object) { return TAGGED(0, object); }
Value boxBoolean(int value)   { return value?True:False; }
Value boxFixnum(long fixnum)  { return TAGGED(1, fixnum); }
Value boxDouble(double value) {
    long v = *(long*)&value;
    return (Value)(v - NAN_MASK);
}

void*  unboxObject(Value value)  { return (void*)value; }
int    unboxBoolean(Value value) { return value == True; }
long   unboxFixnum(Value value)  {
    long data = DATA(value);
    return ((FIXNUM_SIGN&data)?FIXNUM_SIGN_EXT:0) | data;
}
char*  unboxString(Value value)  { return ((struct string*)DATA(value))->string; }
size_t unboxStringLength(Value value)  { return ((struct string*)DATA(value))->length; }
double unboxDouble(Value value)
{
    long v = value + NAN_MASK;
    return *(double*)&v;
}

Value boxString(size_t length, char* string)
{
    struct string* data;

    data = gcAlloc(length+1+sizeof(size_t));
    data->length = length;
    strncpy(data->string, string, length);
    data->string[length] = 0;
    return TAGGED(2, data);
}
