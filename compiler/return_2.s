.text
.globl main

main:
	mov $2, %eax
	push %rax
	mov $2, %eax
	pop %rcx
	ret
