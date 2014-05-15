#!/bin/bash
gcc main.c -o run

gcc -O3 -fPIC -shared interpreter.c value.c gc.c system.c -o runtime.so
