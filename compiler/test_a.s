.text  
.globl main
    main:
        movl $2, %eax
        xor %eax, %eax
        movl $2, %eax
        neg %rax
        ret
