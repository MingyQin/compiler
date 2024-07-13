#!/bin/bash


echo find  *.c 
./compiler return_2.c
gcc -m64 return_2.s
./a.out
echo Output: $?