.text
.globl main

main:
	mov $2, %eax
	push %rax
	mov $3, %eax
	pop %rcx
	add %rcx, %rax
	push %rax
	mov $4, %eax
	pop %rcx
	cqo
	mul %rcx
	ret
