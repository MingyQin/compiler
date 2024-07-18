#!/bin/bash
stage=3

for prog in `find . -type f -name "*.c" -path "./stage_$stage/valid/*" -not -path "*/valid_multifile/*" 2>/dev/null`; do
    # Use my compiler to compile it to assembly
    ./compiler/compiler $prog

    # Get the file name without the extension
    base="${prog%.*}"

    # Compile the assembly to an executable
    gcc -m64 $base.s

    ./a.out
    my_out=$?

    #echo my_out = $my_out

    # Compile using gcc
    gcc $prog

    ./a.out
    correct_out=$?

    #echo correct_out = $correct_out

    if [[ $my_out -eq $correct_out ]];
    then
        echo correct
    else
        echo wrong
    fi
    
done


