#include <stdio.h>
#include "value.h"
#include "gc.h"

typedef unsigned char uint8;

#define OP_MOVE      0
#define OP_LOADGK    1
#define OP_LOADTRUE  2
#define OP_LOADFALSE 3
#define OP_LOADNULL  4
#define OP_GETUPVAL  36
#define OP_GETGLOBAL 37
#define OP_GETDICT   38
#define OP_SETGLOBAL 39
#define OP_SETUPVAL  40
#define OP_SETDICT   41
#define OP_NEWDICT   42
#define OP_SELF      43

#define OP_ADD       44
#define OP_SUB       45
#define OP_MUL       46
#define OP_DIV       47

#define OP_POW       48
#define OP_UNM       49
#define OP_NOT       50
#define OP_CAT       51
#define OP_JUMP      52

#define OP_NE        53
#define OP_EQ        54
#define OP_LT        55
#define OP_LE        56
#define OP_GT        57
#define OP_GE        58

#define OP_TEST      66
#define OP_TESTN     67
#define OP_CALL      68
#define OP_TAILCALL  69
#define OP_RETURN    70
#define OP_FORLOOP   71
#define OP_FORPREP   72
#define OP_TFORLOOP  73
#define OP_TFORPREP  74
#define OP_SETLIST   75
#define OP_CLOSE     76
#define OP_CLOSURE   77
#define OP_VARARG    78

#define OP_LOOP      128

#define OP_UNK       255

double asDouble(Value value)
{
    if (isFixnum(value)) return (double)unboxFixnum(value);
    else return unboxDouble(value);
}

Value interpret(uint8* pc, Value* var, Value* gk, Value** up)
{
    int op, a, b, c, d;
next:
    op = pc[0];
    a  = pc[1];
    b  = pc[2];
    c  = pc[3];
    d  = b << 8 | c;
    pc += 4;
    switch (op)
    {
        case OP_MOVE:      goto op_move;
        case OP_LOADGK:    goto op_loadgk;
        case OP_LOADTRUE:  goto op_loadtrue;
        case OP_LOADFALSE: goto op_loadfalse;
        case OP_LOADNULL:  goto op_loadnull;
        case OP_ADD:  goto op_add;
        case OP_SUB:  goto op_sub;
        case OP_MUL:  goto op_mul;
        case OP_DIV:  goto op_div;
        case OP_JUMP:     goto op_jump;
        case OP_NE:  goto op_ne;
        case OP_EQ:  goto op_eq;
        case OP_LT:  goto op_lt;
        case OP_LE:  goto op_le;
        case OP_GT:  goto op_gt;
        case OP_GE:  goto op_ge;
        case OP_TEST:     goto op_test;
        case OP_TESTN:    goto op_testn;
        case OP_CALL:     goto op_call;
        case OP_RETURN:   goto op_return;
        case OP_LOOP:     goto op_loop;
        default:          goto error;
    }

op_move:
    var[a] = var[b];
    goto next;

op_loadgk: // format ad
    printf("** loadgk\n");
    var[a] = gk[d];
    goto next;

op_loadtrue:
    var[a] = boxBoolean(1);
    printf("** boolean (t)\n");
    goto next;

op_loadfalse:
    var[a] = boxBoolean(0);
    printf("** boolean (f)\n");
    goto next;

op_loadnull: // format abc
    printf("** loadnull\n");
    var[a] = boxObject(0);
    goto next;

op_add:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxFixnum( unboxFixnum(var[b]) + unboxFixnum(var[c]) );
    }
    else if (isDouble(var[b]) || isDouble(var[c]))
    {
        var[a] = boxDouble( asDouble(var[b]) + asDouble(var[c]) );
    }
    else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_sub:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxFixnum( unboxFixnum(var[b]) - unboxFixnum(var[c]) );
    }
    else if (isDouble(var[b]) || isDouble(var[c]))
    {
        var[a] = boxDouble( asDouble(var[b]) - asDouble(var[c]) );
    }
    else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_mul:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxFixnum( unboxFixnum(var[b]) * unboxFixnum(var[c]) );
    }
    else if (isDouble(var[b]) || isDouble(var[c]))
    {
        var[a] = boxDouble( asDouble(var[b]) * asDouble(var[c]) );
    }
    else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_div:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxFixnum( unboxFixnum(var[b]) / unboxFixnum(var[c]) );
    }
    else if (isDouble(var[b]) || isDouble(var[c]))
    {
        var[a] = boxDouble( asDouble(var[b]) / asDouble(var[c]) );
    }
    else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_jump: // format ad
    pc += d - 0x8000;
    goto next;

op_ne:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxBoolean(unboxFixnum(var[b]) != unboxFixnum(var[c]));
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_eq:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxBoolean(unboxFixnum(var[b]) == unboxFixnum(var[c]));
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_lt:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxBoolean(unboxFixnum(var[b]) < unboxFixnum(var[c]));
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_le:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxBoolean(unboxFixnum(var[b]) <= unboxFixnum(var[c]));
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_gt:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        printf("** GT %li > %li is ", unboxFixnum(var[b]), unboxFixnum(var[c]));
        var[a] = boxBoolean(unboxFixnum(var[b]) > unboxFixnum(var[c]));
        printf("%s\n", isTrue(var[a])?"true":"false");
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_ge:
    if (isFixnum(var[b]) && isFixnum(var[c]))
    {
        var[a] = boxBoolean(unboxFixnum(var[b]) >= unboxFixnum(var[c]));
    } else
    {
        error(1, 0, "no impl for %i", op);
    }
    goto next;

op_test: // format ad
    printf("** test\n");
    if (!(isNull(var[a]) || isFalse(var[a])))
    {
        printf("** pc += %i\n", d - 0x8000);
        pc += d - 0x8000;
    }
    goto next;

op_testn: // format ad
    printf("** testn\n");
    if (isNull(var[a]) || isFalse(var[a]))
    {
        printf("** pc += %i\n", d - 0x8000);
        pc += d - 0x8000;
    }
    goto next;

op_call: // format abc
    {
        printf("** call\n");

        if (!isObject(var[b]))
        {
            error(1, 0, "not a callable object");
        }
        struct system_func* f = unboxObject(var[b]);
        if (f->object.type != T_SYSTEM_FUNCTION)
        {
            error(1, 0, "cannot call non-system function yet");
        }
        var[a] = f->call(c-1, var+b+1);

    }
    goto next;

op_return:
    printf("** return\n");
    return var[a]; 

op_loop:
    printf("** loop\n");
    goto next;

error:
    error(1, 0, "bad instruction %i a=%i b=%i c=%i d=%i\n", op, a, b, c, d);
}
