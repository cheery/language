#include "snakelisp.h"

static void f101(closure_t*, argv_t*) c_noreturn;
int main(int argc, char** argv)
{
  c_boot(f101);
  return 0;
}

static void f101(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_string("hello hello\n");
  c_call_begin(4);
  c_call_argument(0, *&cl_file_write);
  c_call_argument(1, *&a1);
  c_call_argument(2, *&c0);
  c_call_argument(3, *&v_stdout);
  c_call_end();
}
