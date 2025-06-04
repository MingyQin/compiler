#!/bin/bash
make compiler
./compiler test.c
gcc -m64 test.s
./a.out
echo Output: $?
