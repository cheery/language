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
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f102));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  value_t vf108 = spawnClosure(f108, );
  call(*&v_pick, *SLOT(0), *&a1, *SLOT(1), *&vf108);
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v101, v102, v103;
  value_t vf107 = spawnClosure(f107, &a1/*cont*/, &v102/*count*/, &v101/*fd*/, &v103/*buffer*/);
  *&v101/*fd*/ = *&c1;
  *&v103/*buffer*/ = *&c1;
  *&v102/*count*/ = *&c1;
  call(*&v_file_open, *&vf107, *&c0);
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t vf106 = spawnClosure(f106, SLOT(0)/*cont*/, SLOT(1)/*count*/, SLOT(3)/*fd*/, SLOT(2)/*buffer*/);
  *SLOT(2)/*buffer*/ = *&a1;
  call(*&v_file_read, *&vf106, *SLOT(3)/*fd*/, *SLOT(2)/*buffer*/);
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  call(*&v_file_close, *SLOT(0)/*cont*/, *SLOT(1)/*fd*/);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t v104;
  value_t vf108 = spawnClosure(f108, );
  value_t vf110 = spawnClosure(f110, SLOT(1)/*count*/, SLOT(3)/*fd*/, &v104, SLOT(2)/*buffer*/);
  value_t vf104 = spawnClosure(f104, SLOT(0)/*cont*/, SLOT(3)/*fd*/);
  *&v104 = *&vf110;
  call(*&v_pick, *&vf104, *&a1, *&v104, *&vf108);
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t vf105 = spawnClosure(f105, SLOT(0)/*cont*/, SLOT(1)/*count*/, SLOT(3)/*buffer*/, SLOT(2)/*fd*/);
  *SLOT(1)/*count*/ = *&a1;
  call(*&v_lt, *&vf105, *&c0, *&a1);
}

CONTINUATION(f107)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(262);
  value_t vf103 = spawnClosure(f103, SLOT(0)/*cont*/, SLOT(1)/*count*/, SLOT(3)/*buffer*/, SLOT(2)/*fd*/);
  *SLOT(2)/*fd*/ = *&a1;
  call(*&v_arraybuffer, *&vf103, *&c0);
}

CONTINUATION(f108)
{
  value_t a1 = ARG(1);
  value_t c0 = boxNull();
  call(*&a1, *&c0);
}

CONTINUATION(f109)
{
  value_t a1 = ARG(1);
  value_t vf111 = spawnClosure(f111, SLOT(1), SLOT(3));
  call(*&v_file_read, *&vf111, *SLOT(0)/*fd*/, *SLOT(2)/*buffer*/);
}

CONTINUATION(f110)
{
  value_t a1 = ARG(1);
  value_t vf109 = spawnClosure(f109, SLOT(1)/*fd*/, &a1, SLOT(3)/*buffer*/, SLOT(2));
  call(*&v_file_write, *&vf109, *&v_stdout, *SLOT(3)/*buffer*/, *SLOT(0)/*count*/);
}

CONTINUATION(f111)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(0);
  value_t v102;
  value_t vf101 = spawnClosure(f101, SLOT(0), SLOT(1));
  *&v102/*count*/ = *&a1;
  call(*&v_lt, *&vf101, *&c0, *&a1);
}
