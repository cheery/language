/* generated from: demo */
#include "snakelisp.h"

extern value_t v_and;
extern value_t v_floordiv;
extern value_t v_file_write;
extern value_t v_gt;
extern value_t v_store_idx;
extern value_t v_stdout;
extern value_t v_ne;
extern value_t v_is_boolean;
extern value_t v_is_closure;
extern value_t v_is_integer;
extern value_t v_is_arraybuffer;
extern value_t v_ge;
extern value_t v_file_open;
extern value_t v_not;
extern value_t v_modulus;
extern value_t v_array;
extern value_t v_eq;
extern value_t v_to_ordinal;
extern value_t v_mul;
extern value_t v_or;
extern value_t v_file_read;
extern value_t v_le;
extern value_t v_is_array;
extern value_t v_load_idx;
extern value_t v_is_null;
extern value_t v_arraybuffer;
extern value_t v_stdin;
extern value_t v_sub;
extern value_t v_is_true;
extern value_t v_div;
extern value_t v_cat;
extern value_t v_is_string;
extern value_t v_length;
extern value_t v_lt;
extern value_t v_is_double;
extern value_t v_stderr;
extern value_t v_pick;
extern value_t v_is_false;
extern value_t v_to_character;
extern value_t v_add;
extern value_t v_call_cc;
extern value_t v_file_close;
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
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f103));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  value_t vf102 = spawnClosure(f102, );
  call(*&v_pick, *SLOT(0), *&a1, *SLOT(1), *&vf102);
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v101, v102, v103;
  value_t vf107 = spawnClosure(f107, &v103/*buffer*/, &v102/*fd*/, &a1/*cont*/, &v101/*count*/);
  *&v102/*fd*/ = *&c1;
  *&v103/*buffer*/ = *&c1;
  *&v101/*count*/ = *&c1;
  call(*&v_file_open, *&vf107, *&c0);
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  value_t v104;
  value_t vf102 = spawnClosure(f102, );
  value_t vf110 = spawnClosure(f110, SLOT(2)/*count*/, SLOT(0)/*buffer*/, SLOT(3)/*fd*/, &v104);
  value_t vf106 = spawnClosure(f106, SLOT(1)/*cont*/, SLOT(3)/*fd*/);
  *&v104 = *&vf110;
  call(*&v_pick, *&vf106, *&a1, *&v104, *&vf102);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t vf109 = spawnClosure(f109, SLOT(1)/*count*/, SLOT(3)/*buffer*/, SLOT(0)/*fd*/, SLOT(2)/*cont*/);
  *SLOT(3)/*buffer*/ = *&a1;
  call(*&v_file_read, *&vf109, *SLOT(0)/*fd*/, *SLOT(3)/*buffer*/);
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  call(*&v_file_close, *SLOT(0)/*cont*/, *SLOT(1)/*fd*/);
}

CONTINUATION(f107)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(262);
  value_t vf105 = spawnClosure(f105, SLOT(1)/*fd*/, SLOT(3)/*count*/, SLOT(2)/*cont*/, SLOT(0)/*buffer*/);
  *SLOT(1)/*fd*/ = *&a1;
  call(*&v_arraybuffer, *&vf105, *&c0);
}

CONTINUATION(f108)
{
  value_t a1 = ARG(1);
  value_t vf111 = spawnClosure(f111, SLOT(1), SLOT(3), SLOT(4)/*count*/);
  call(*&v_file_read, *&vf111, *SLOT(0)/*fd*/, *SLOT(2)/*buffer*/);
}

CONTINUATION(f109)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t vf104 = spawnClosure(f104, SLOT(1)/*buffer*/, SLOT(3)/*cont*/, SLOT(0)/*count*/, SLOT(2)/*fd*/);
  *SLOT(0)/*count*/ = *&a1;
  call(*&v_lt, *&vf104, *&c0, *SLOT(0)/*count*/);
}

CONTINUATION(f110)
{
  value_t a1 = ARG(1);
  value_t vf108 = spawnClosure(f108, SLOT(2)/*fd*/, SLOT(3), SLOT(1)/*buffer*/, &a1, SLOT(0)/*count*/);
  call(*&v_file_write, *&vf108, *&v_stdout, *SLOT(1)/*buffer*/, *SLOT(0)/*count*/);
}

CONTINUATION(f111)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t vf101 = spawnClosure(f101, SLOT(1), SLOT(0));
  *SLOT(2)/*count*/ = *&a1;
  call(*&v_lt, *&vf101, *&c0, *SLOT(2)/*count*/);
}
