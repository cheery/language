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
extern value_t v_call_cc;
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
static CONTINUATION(f126);
static CONTINUATION(f127);
static CONTINUATION(f128);
static CONTINUATION(f129);
static CONTINUATION(f130);
static CONTINUATION(f131);
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f123));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  value_t vf129 = spawnClosure(f129, SLOT(0), SLOT(1));
  value_t vf130 = spawnClosure(f130, SLOT(0));
  value_t vf115 = spawnClosure(f115, SLOT(0));
  call(*&v_pick, *&vf129, *&a1, *&vf115, *&vf130);
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("can't be!");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(4096);
  value_t v101;
  value_t vf131 = spawnClosure(f131, &v101, SLOT(0));
  *&v101 = *&a1;
  call(*&v_arraybuffer, *&vf131, *&c0);
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  value_t vf101 = spawnClosure(f101, SLOT(0), SLOT(1));
  call(*&v_is_null, *&vf101, *&c0);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t a2 = ARG(2);
  value_t c0 = spawnString("\n");
  value_t vf109 = spawnClosure(f109, &a1);
  call(*&v_cat, *&vf109, *&a2, *&c0);
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("hello hello\n");
  value_t vf122 = spawnClosure(f122, SLOT(0));
  call(*&v_file_write, *&vf122, *&v_stdout, *&c0);
}

CONTINUATION(f107)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("no.");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f108)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("no jello");
  call(*SLOT(0), *SLOT(1), *&c0);
}

CONTINUATION(f109)
{
  value_t a1 = ARG(1);
  call(*&v_file_write, *SLOT(0), *&v_stdout, *&a1);
}

CONTINUATION(f110)
{
  value_t a1 = ARG(1);
  value_t v102;
  value_t vf125 = spawnClosure(f125, &v102, SLOT(2), SLOT(1));
  *&v102 = *&a1;
  call(*&v_file_close, *&vf125, *SLOT(0));
}

CONTINUATION(f111)
{
  value_t a1 = ARG(1);
  value_t vf119 = spawnClosure(f119, SLOT(0), SLOT(1));
  value_t vf118 = spawnClosure(f118, SLOT(0));
  value_t vf116 = spawnClosure(f116, );
  call(*&v_pick, *&vf119, *&a1, *&vf118, *&vf116);
}

CONTINUATION(f112)
{
  value_t a1 = ARG(1);
  value_t vf111 = spawnClosure(f111, SLOT(0), SLOT(1));
  call(*&v_is_null, *&vf111, *&a1);
}

CONTINUATION(f113)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("maybe!");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f114)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  value_t vf112 = spawnClosure(f112, SLOT(0), SLOT(1));
  call(*&vf112, *&c0);
}

CONTINUATION(f115)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("null is null");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f116)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}

CONTINUATION(f117)
{
  value_t a1 = ARG(1);
  value_t c0 = boxTrue();
  value_t vf114 = spawnClosure(f114, SLOT(0), SLOT(1));
  value_t vf128 = spawnClosure(f128, SLOT(0));
  value_t vf107 = spawnClosure(f107, SLOT(0));
  call(*&v_pick, *&vf114, *&c0, *&vf128, *&vf107);
}

CONTINUATION(f118)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("sad cond :<");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f119)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("odd cond. :>");
  value_t vf104 = spawnClosure(f104, SLOT(0), SLOT(1));
  call(*SLOT(0), *&vf104, *&c0);
}

CONTINUATION(f120)
{
  value_t a1 = ARG(1);
  value_t c0 = boxFalse();
  value_t vf117 = spawnClosure(f117, SLOT(0), SLOT(1));
  value_t vf113 = spawnClosure(f113, SLOT(0));
  value_t vf102 = spawnClosure(f102, SLOT(0));
  call(*&v_pick, *&vf117, *&c0, *&vf102, *&vf113);
}

CONTINUATION(f121)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("good jello");
  value_t vf124 = spawnClosure(f124, &a1, SLOT(1));
  call(*SLOT(0), *&vf124, *&c0);
}

CONTINUATION(f122)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("hello snake");
  value_t v103;
  value_t vf120 = spawnClosure(f120, &v103, SLOT(0));
  value_t vf105 = spawnClosure(f105, );
  *&v103 = *&vf105;
  call(*&v103, *&vf120, *&c0);
}

CONTINUATION(f123)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v102, v104, v103, v101, v105;
  value_t vf103 = spawnClosure(f103, &a1);
  *&v101 = *&c1;
  *&v105 = *&c1;
  *&v102 = *&c1;
  *&v103 = *&c1;
  *&v104 = *&c1;
  call(*&v_file_open, *&vf103, *&c0);
}

CONTINUATION(f124)
{
  value_t a1 = ARG(1);
  value_t c0 = boxFalse();
  value_t v104;
  value_t vf127 = spawnClosure(f127, );
  *&v104 = *&c0;
  call(*&v_pick, *SLOT(0), *&v104, *SLOT(1), *&vf127);
}

CONTINUATION(f125)
{
  value_t a1 = ARG(1);
  value_t vf106 = spawnClosure(f106, SLOT(1));
  call(*&v_file_write, *&vf106, *&v_stdout, *SLOT(2), *SLOT(0));
}

CONTINUATION(f126)
{
  value_t a1 = ARG(1);
  value_t v106;
  value_t vf121 = spawnClosure(f121, SLOT(2), &v106);
  value_t vf127 = spawnClosure(f127, );
  value_t vf108 = spawnClosure(f108, SLOT(2), SLOT(1));
  *&v106 = *&vf121;
  call(*&v_pick, *&vf108, *SLOT(0), *&v106, *&vf127);
}

CONTINUATION(f127)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}

CONTINUATION(f128)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("yes.");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f129)
{
  value_t a1 = ARG(1);
  value_t c0 = boxTrue();
  value_t c1 = spawnString("test jello");
  value_t v104;
  value_t vf126 = spawnClosure(f126, &v104, SLOT(1), SLOT(0));
  *&v104 = *&c0;
  call(*SLOT(0), *&vf126, *&c1);
}

CONTINUATION(f130)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("weeird");
  call(*SLOT(0), *&a1, *&c0);
}

CONTINUATION(f131)
{
  value_t a1 = ARG(1);
  value_t v105;
  value_t vf110 = spawnClosure(f110, SLOT(0), &v105, SLOT(1));
  *&v105 = *&a1;
  call(*&v_file_read, *&vf110, *SLOT(0), *&v105);
}
