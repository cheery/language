#include "vm.h"

int vm_cmp(vm_context *ctx, vm_val a, vm_val b)
{
    vm_object_type type_a, type_b;

    size_t len_a, len_b, len;
    vm_string *as, *bs;
    const char *astr;
    const char *bstr;
    double anum, bnum;
    int i;

    if (a == b) return 0;

    type_a = vm_typeof(a);
    type_b = vm_typeof(b);
    if (type_a == type_b) switch (type_a)
    {
        case vm_t_string:
            as = vm_unbox(ctx, a, vm_t_string);
            bs = vm_unbox(ctx, b, vm_t_string);
                
            len_a = as->length;
            len_b = bs->length;
            len = (len_b < len_a)?len_b:len_a;

            astr = as->data;
            bstr = bs->data;
            for(i = 0; i < len; i++)
            {
                if (astr[i] < bstr[i])
                {
                    return -1;
                }
                else if (astr[i] > bstr[i])
                {
                    return +1;
                }
            }
            if (len_a < len_b)
            {
                return -1;
            }
            else if (len_a > len_b)
            {
                return +1;
            }
            else
            {
                return 0;
            }
        case vm_t_double:
            anum = vm_unbox_double(ctx, a);
            bnum = vm_unbox_double(ctx, b);
            if (anum < bnum)
            {
                return -1;
            }
            else if (anum > bnum)
            {
                return +1;
            }
            else
            {
                return 0;
            }
        default:
            break;
    }

    if (a < b)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}
