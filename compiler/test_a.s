.text  
.globl main
    main:
        mov $2, %rax 
        add $2, %rax
        push %rax
        pop %rax
        add $2, %rax
        ret
