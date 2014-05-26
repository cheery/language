#include <stdint.h>
#include <stdlib.h>
#include "value.h"
#include "cmp.h"

int vm_cmp(vm_value a, vm_value b)
{
    vm_tag tag_a, tag_b;
    size_t len_a, len_b, len;
    char* astr;
    char* bstr;
    double anum, bnum;
    int i;

    if (a == b) return 0;

    tag_a = vm_unbox_tag(a);
    tag_b = vm_unbox_tag(b);
    if (tag_a == tag_b) switch (tag_a)
    {
        case vm_tag_object:
            break;
        case vm_tag_string:
            len_a = vm_string_length(a);
            len_b = vm_string_length(b);
            len = (len_b < len_a)?len_b:len_a;

            astr = vm_unbox_string(a);
            bstr = vm_unbox_string(b);
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
        case vm_tag_double:
            anum = vm_unbox_double(a);
            bnum = vm_unbox_double(b);
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
        case vm_tag_constant:
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
