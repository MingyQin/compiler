#!/bin/bash
make compiler
valgrind ./compiler test.c
gcc -m64 test.s
./a.out
echo Output: $?