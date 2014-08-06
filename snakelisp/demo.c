/* generated from: demo */
#include "snakelisp.h"

extern value_t v_store_idx;
extern value_t v_stdout;
extern value_t v_is_boolean;
extern value_t v_file_write;
extern value_t v_array;
extern value_t v_is_array;
extern value_t v_is_null;
extern value_t v_is_integer;
extern value_t v_is_string;
extern value_t v_is_true;
extern value_t v_is_double;
extern value_t v_file_close;
extern value_t v_is_closure;
extern value_t v_is_arraybuffer;
extern value_t v_file_open;
extern value_t v_file_read;
extern value_t v_load_idx;
extern value_t v_arraybuffer;
extern value_t v_stdin;
extern value_t v_cat;
extern value_t v_length;
extern value_t v_stderr;
extern value_t v_pick;
extern value_t v_is_false;
static CONTINUATION(f101);
static CONTINUATION(f102);
static CONTINUATION(f103);
static CONTINUATION(f104);
static CONTINUATION(f105);
static CONTINUATION(f106);
static CONTINUATION(f107);
static CONTINUATION(f108);
static CONTINUATION(f109);
static CONTINUATION(f110);
static CONTINUATION(f111);
static CONTINUATION(f112);
static CONTINUATION(f113);
static CONTINUATION(f114);
static CONTINUATION(f115);
static CONTINUATION(f116);
static CONTINUATION(f117);
static CONTINUATION(f118);
static CONTINUATION(f119);
static CONTINUATION(f120);
static CONTINUATION(f121);
static CONTINUATION(f122);
static CONTINUATION(f123);
static CONTINUATION(f124);
static CONTINUATION(f125);
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f115));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  value_t c0 = boxFalse();
  value_t vf123 = spawnClosure(f123, SLOT(0), SLOT(1));
  value_t vf122 = spawnClosure(f122, SLOT(0));
  value_t vf118 = spawnClosure(f118, SLOT(0));
  call(*&v_pick, *&vf123, *&c0, *&vf118, *&vf122);
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t v101;
  value_t vf112 = spawnClosure(f112, SLOT(0), &v101, SLOT(2));
  *&v101 = *&a1;
  call(*&v_file_close, *&vf112, *SLOT(1));
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t vf116 = spawnClosure(f116, SLOT(0), SLOT(1));
  call(*&v_is_null, *&vf116, *&a1);
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  value_t v102;
  value_t vf102 = spawnClosure(f102, SLOT(0), SLOT(1), &v102);
  *&v102 = *&a1;
  call(*&v_file_read, *&vf102, *SLOT(1), *&v102);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t vf107 = spawnClosure(f107, SLOT(1));
  value_t vf114 = spawnClosure(f114, SLOT(1));
  call(*&v_pick, *SLOT(0), *&a1, *&vf107, *&vf114);
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("yes.");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f107)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("null is null");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f108)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(4096);
  value_t v103;
  value_t vf104 = spawnClosure(f104, SLOT(0), &v103);
  *&v103 = *&a1;
  call(*&v_arraybuffer, *&vf104, *&c0);
}

CONTINUATION(f109)
{
  value_t a1 = ARG(1);
  value_t a2 = ARG(2);
  value_t c0 = spawnString("\n");
  value_t vf113 = spawnClosure(f113, &a1);
  call(*&v_cat, *&vf113, *&a2, *&c0);
}

CONTINUATION(f110)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  value_t vf105 = spawnClosure(f105, SLOT(0), SLOT(1));
  call(*&v_is_null, *&vf105, *&c0);
}

CONTINUATION(f111)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("no.");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f112)
{
  value_t a1 = ARG(1);
  value_t vf121 = spawnClosure(f121, SLOT(0));
  call(*&v_file_write, *&vf121, *&v_stdout, *SLOT(2), *SLOT(1));
}

CONTINUATION(f113)
{
  value_t a1 = ARG(1);
  call(*&v_file_write, *SLOT(0), *&v_stdout, *&a1);
}

CONTINUATION(f114)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("weeird");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f115)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v104, v103, v101, v102;
  value_t vf108 = spawnClosure(f108, &a1);
  *&v103 = *&c1;
  *&v102 = *&c1;
  *&v101 = *&c1;
  *&v104 = *&c1;
  call(*&v_file_open, *&vf108, *&c0);
}

CONTINUATION(f116)
{
  value_t a1 = ARG(1);
  value_t vf120 = spawnClosure(f120, SLOT(0));
  value_t vf117 = spawnClosure(f117, SLOT(0), SLOT(1));
  value_t vf125 = spawnClosure(f125, );
  call(*&v_pick, *&vf117, *&a1, *&vf120, *&vf125);
}

CONTINUATION(f117)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("odd cond. :>");
  value_t vf110 = spawnClosure(f110, SLOT(1), SLOT(0));
  call(*SLOT(0), *&vf110, *&c0);
}

CONTINUATION(f118)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("can't be!");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f119)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("hello snake");
  value_t v104;
  value_t vf109 = spawnClosure(f109, );
  value_t vf101 = spawnClosure(f101, &v104, SLOT(0));
  *&v104 = *&vf109;
  call(*&v104, *&vf101, *&c0);
}

CONTINUATION(f120)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("sad cond :<");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f121)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("hello hello\n");
  value_t vf119 = spawnClosure(f119, SLOT(0));
  call(*&v_file_write, *&vf119, *&v_stdout, *&c0);
}

CONTINUATION(f122)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("maybe!");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f123)
{
  value_t a1 = ARG(1);
  value_t c0 = boxTrue();
  value_t vf106 = spawnClosure(f106, SLOT(0));
  value_t vf124 = spawnClosure(f124, SLOT(0), SLOT(1));
  value_t vf111 = spawnClosure(f111, SLOT(0));
  call(*&v_pick, *&vf124, *&c0, *&vf106, *&vf111);
}

CONTINUATION(f124)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  value_t vf103 = spawnClosure(f103, SLOT(0), SLOT(1));
  call(*&vf103, *&c0);
}

CONTINUATION(f125)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}
