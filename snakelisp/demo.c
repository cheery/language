/* generated from: demo */
#include "snakelisp.h"

extern value_t v_file_read;
extern value_t v_stdout;
extern value_t v_stderr;
extern value_t v_file_open;
extern value_t v_pick;
extern value_t v_arraybuffer;
extern value_t v_stdin;
extern value_t v_file_write;
extern value_t v_file_close;
static CONTINUATION(f101);
static CONTINUATION(f102);
static CONTINUATION(f103);
static CONTINUATION(f104);
static CONTINUATION(f105);
static CONTINUATION(f106);
void main(int argc, char** argv)
{
  snakeBoot(spawnClosure(f104));
}

CONTINUATION(f101)
{
  value_t a1 = ARG(1);
  value_t v101;
  value_t vf105 = spawnClosure(f105, &v101, SLOT(1), SLOT(0));
  *&v101 = *&a1;
  call(*&v_file_read, *&vf105, *SLOT(1), *&v101);
}

CONTINUATION(f102)
{
  value_t a1 = ARG(1);
  value_t c0 = boxInteger(4096);
  value_t v102;
  value_t vf101 = spawnClosure(f101, SLOT(0), &v102);
  *&v102 = *&a1;
  call(*&v_arraybuffer, *&vf101, *&c0);
}

CONTINUATION(f103)
{
  value_t a1 = ARG(1);
  value_t vf106 = spawnClosure(f106, SLOT(1));
  call(*&v_file_write, *&vf106, *&v_stdout, *SLOT(0), *SLOT(2));
}

CONTINUATION(f104)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("demo");
  value_t c1 = boxNull();
  value_t v102, v103, v101;
  value_t vf102 = spawnClosure(f102, &a1);
  *&v102 = *&c1;
  *&v101 = *&c1;
  *&v103 = *&c1;
  call(*&v_file_open, *&vf102, *&c0);
}

CONTINUATION(f105)
{
  value_t a1 = ARG(1);
  value_t v103;
  value_t vf103 = spawnClosure(f103, SLOT(0), SLOT(2), &v103);
  *&v103 = *&a1;
  call(*&v_file_close, *&vf103, *SLOT(1));
}

CONTINUATION(f106)
{
  value_t a1 = ARG(1);
  value_t c0 = spawnString("hello hello\n");
  call(*&v_file_write, *SLOT(0), *&v_stdout, *&c0);
}
