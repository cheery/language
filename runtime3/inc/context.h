typedef struct vm_descriptor vm_descriptor;
struct vm_descriptor
{
    vm_object object;
    size_t argc;
    size_t valc;
    size_t upvalc;
    size_t upcopyc;
    uint16_t* upcopy;
    size_t   size;
    uint8_t* program;
    vm_value functions;
    vm_value constants;
};

typedef struct
{
    vm_object object;
    vm_descriptor* desc;
    vm_value* upvalues[];
} vm_closure;

typedef struct
{
    vm_descriptor* desc;
    vm_closure* closure;
    vm_value **upvalues;
    uint8_t*  pc;
    vm_value* base;
    vm_value* top;
} vm_frame;

struct vm_context
{
    int frame;
    vm_value* cbase;
    vm_value* ctop;
    vm_value  stack[4096];
    vm_frame frames[1024];
};
