.text
.globl main

main:
	mov $12, %eax
	neg %rax
	push %rax
	mov $5, %eax
	pop %rcx
	xchg %rax, %rcx
	cqo
	idiv %rcx
	ret
