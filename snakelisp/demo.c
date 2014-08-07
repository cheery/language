/* generated from: demo */
#include "snakelisp.h"

extern value_t v_gt;
extern value_t v_store_idx;
extern value_t v_stdout;
extern value_t v_is_boolean;
extern value_t v_file_write;
extern value_t v_ne;
extern value_t v_ge;
extern value_t v_array;
extern value_t v_eq;
extern value_t v_le;
extern value_t v_is_array;
extern value_t v_is_null;
extern value_t v_is_integer;
extern value_t v_is_string;
extern value_t v_is_true;
extern value_t v_lt;
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
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f112));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  call(*&v_file_close, *SLOT(0), *SLOT(1));
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t v101;
  value_t vf110 = spawnClosure(f110, SLOT(0), SLOT(2), &v101, SLOT(1));
  *&v101 = *&a1;
  call(*&v_file_read, *&vf110, *SLOT(0), *&v101);
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  value_t a2 = ARG(2);
  value_t c0 = spawnString("\n");
  value_t vf107 = spawnClosure(f107, &a1);
  call(*&v_cat, *&vf107, *&a2, *&c0);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t v102;
  value_t vf111 = spawnClosure(f111, &v102, SLOT(2), SLOT(3), SLOT(1), SLOT(0));
  value_t vf115 = spawnClosure(f115, SLOT(2), SLOT(0), SLOT(4));
  value_t vf103 = spawnClosure(f103, );
  *&v102 = *&vf111;
  call(*&v_pick, *&vf115, *&a1, *&v102, *&vf103);
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t vf109 = spawnClosure(f109, SLOT(0), SLOT(1));
  call(*&v_lt, *&vf109, *&c0, *SLOT(2));
}

CONTINUATION(f107)
{
  value_t a1 = ARG(1);
  call(*&v_file_write, *SLOT(0), *&v_stdout, *&a1);
}

CONTINUATION(f108)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(128);
  value_t v103;
  value_t vf102 = spawnClosure(f102, &v103, SLOT(1), SLOT(0));
  *&v103 = *&a1;
  call(*&v_arraybuffer, *&vf102, *&c0);
}

CONTINUATION(f109)
{
  value_t a1 = ARG(1);
  value_t vf103 = spawnClosure(f103, );
  call(*&v_pick, *SLOT(1), *&a1, *SLOT(0), *&vf103);
}

CONTINUATION(f110)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t v104;
  value_t vf105 = spawnClosure(f105, SLOT(1), SLOT(2), SLOT(0), &v104, SLOT(3));
  *&v104 = *&a1;
  call(*&v_lt, *&vf105, *&c0, *&v104);
}

CONTINUATION(f111)
{
  value_t a1 = ARG(1);
  value_t vf114 = spawnClosure(f114, SLOT(0), SLOT(1), SLOT(3), SLOT(4), &a1);
  call(*&v_file_write, *&vf114, *&v_stdout, *SLOT(3), *SLOT(2));
}

CONTINUATION(f112)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v101, v104, v105, v103;
  value_t vf104 = spawnClosure(f104, );
  value_t vf108 = spawnClosure(f108, &v105, &a1);
  *&v105 = *&c1;
  *&v103 = *&c1;
  *&v101 = *&c1;
  *&v104 = *&c1;
  *&v105 = *&vf104;
  call(*&v_file_open, *&vf108, *&c0);
}

CONTINUATION(f113)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("read cycle");
  value_t v104;
  value_t vf106 = spawnClosure(f106, SLOT(0), SLOT(2), &v104);
  *&v104 = *&a1;
  call(*SLOT(1), *&vf106, *&c0);
}

CONTINUATION(f114)
{
  value_t a1 = ARG(1);
  value_t vf113 = spawnClosure(f113, SLOT(0), SLOT(3), SLOT(4));
  call(*&v_file_read, *&vf113, *SLOT(1), *SLOT(2));
}

CONTINUATION(f115)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("done");
  value_t vf101 = spawnClosure(f101, SLOT(2), SLOT(0));
  call(*SLOT(1), *&vf101, *&c0);
}
