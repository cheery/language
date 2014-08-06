#include "snakelisp.h"

static void f101(closure_t*, argv_t*) c_noreturn;
static void f102(closure_t*, argv_t*) c_noreturn;
static void f103(closure_t*, argv_t*) c_noreturn;
static void f104(closure_t*, argv_t*) c_noreturn;
static void f105(closure_t*, argv_t*) c_noreturn;
static void f106(closure_t*, argv_t*) c_noreturn;
int main(int argc, char** argv)
{
  c_boot(f104);
  return 0;
}

static void f101(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  c_alloc_closure(f105, cf105, vf105, 3);
  value_t v101;
  *&v101 = *&a1;
  c_call_begin(4);
  c_closure_lift(cf105, 0, &v101);
  c_closure_lift(cf105, 1, c_slot(closure, 0));
  c_closure_lift(cf105, 2, c_slot(closure, 1));
  c_call_argument(0, *&cl_file_read);
  c_call_argument(1, *&vf105);
  c_call_argument(2, *c_slot(closure, 0));
  c_call_argument(3, *&v101);
  c_call_end();
}

static void f102(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_integer(4096);
  c_alloc_closure(f101, cf101, vf101, 2);
  value_t v102;
  *&v102 = *&a1;
  c_call_begin(3);
  c_closure_lift(cf101, 0, &v102);
  c_closure_lift(cf101, 1, c_slot(closure, 0));
  c_call_argument(0, *&cl_arraybuffer);
  c_call_argument(1, *&vf101);
  c_call_argument(2, *&c0);
  c_call_end();
}

static void f103(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  c_alloc_closure(f106, cf106, vf106, 1);
  c_call_begin(5);
  c_closure_lift(cf106, 0, c_slot(closure, 1));
  c_call_argument(0, *&cl_file_write);
  c_call_argument(1, *&vf106);
  c_call_argument(2, *&v_stdout);
  c_call_argument(3, *c_slot(closure, 0));
  c_call_argument(4, *c_slot(closure, 2));
  c_call_end();
}

static void f104(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_string("demo");
  value_t c1 = c_const_null();
  c_alloc_closure(f102, cf102, vf102, 1);
  value_t v101, v102, v103;
  *&v102 = *&c1;
  *&v101 = *&c1;
  *&v103 = *&c1;
  c_call_begin(3);
  c_closure_lift(cf102, 0, &a1);
  c_call_argument(0, *&cl_file_open);
  c_call_argument(1, *&vf102);
  c_call_argument(2, *&c0);
  c_call_end();
}

static void f105(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  c_alloc_closure(f103, cf103, vf103, 3);
  value_t v103;
  *&v103 = *&a1;
  c_call_begin(3);
  c_closure_lift(cf103, 0, c_slot(closure, 0));
  c_closure_lift(cf103, 1, c_slot(closure, 2));
  c_closure_lift(cf103, 2, &v103);
  c_call_argument(0, *&cl_file_close);
  c_call_argument(1, *&vf103);
  c_call_argument(2, *c_slot(closure, 1));
  c_call_end();
}

static void f106(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_string("hello hello\n");
  c_call_begin(4);
  c_call_argument(0, *&cl_file_write);
  c_call_argument(1, *c_slot(closure, 0));
  c_call_argument(2, *&v_stdout);
  c_call_argument(3, *&c0);
  c_call_end();
}
