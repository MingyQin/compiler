#!/bin/bash
make compiler
./compiler test.c
gcc -m64 test.s
./a.out
echo Output: $?

gcc test.c
./a.out
echo Correct: $?