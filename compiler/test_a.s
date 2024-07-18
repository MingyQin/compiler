.text  
.globl main

main:
	mov $12, %rax
    neg %rax
    mov $5, %rcx
    cqo
    div %rcx
	ret
