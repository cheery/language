void* gcAlloc(unsigned long size);

struct object
{
    int type;
};
typedef struct object* Object;

#define T_SYSTEM_FUNCTION 1
#define OBJECT_HEADER struct object object;

struct system_func
{
    OBJECT_HEADER;
    Value (*call)(long argc, Value* argv);
}; 
