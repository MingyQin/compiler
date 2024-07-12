.text
.globl main

main:
	mov $6, %rax
	push %rax
	mov $3, %rax
	pop %rcx
	xchg %rax, %rcx
	cqo
	div %rcx
	ret
