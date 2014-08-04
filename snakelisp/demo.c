#include "snakelisp.h"

static void f101(closure_t*, argv_t*) c_noreturn;
static void f102(closure_t*, argv_t*) c_noreturn;
static void f103(closure_t*, argv_t*) c_noreturn;
static void f104(closure_t*, argv_t*) c_noreturn;
int main(int argc, char** argv)
{
  c_boot(f103);
  return 0;
}

static void f101(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t a2 = c_get_argument(args, 2);
  value_t c0 = c_const_integer(5);
  c_call_begin(2);
  c_call_argument(0, *&a1);
  c_call_argument(1, *&c0);
  c_call_end();
}

static void f102(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  c_call_begin(2);
  c_call_argument(0, *&a1);
  c_call_argument(1, *c_slot(closure, 0));
  c_call_end();
}

static void f103(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_integer(2);
  value_t c1 = c_const_integer(15);
  value_t c2 = c_const_null();
  c_alloc_closure(f101, cf101, vf101, 0);
  c_alloc_closure(f104, cf104, vf104, 1);
  c_alloc_closure(f102, cf102, vf102, 1);
  value_t v101, v102;
  *&v102 = *&c2;
  *&v101 = *&c2;
  *&v102 = *&c0;
  *&v101 = *&vf101;
  c_call_begin(3);
  c_closure_lift(cf104, 0, &v101);
  c_closure_lift(cf102, 0, &a1);
  c_call_argument(0, *&vf104);
  c_call_argument(1, *&vf102);
  c_call_argument(2, *&c1);
  c_call_end();
}

static void f104(closure_t* closure, argv_t* args)
{
  value_t a1 = c_get_argument(args, 1);
  value_t c0 = c_const_integer(3);
  value_t v102;
  *&v102 = *&c0;
  c_call_begin(2);
  c_call_argument(0, *&a1);
  c_call_argument(1, *c_slot(closure, 0));
  c_call_end();
}
