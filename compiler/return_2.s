.text
.globl main

main:
	mov $2, %eax
	push %rax
	mov $2, %eax
	pop %rcx
	cqo
	imul %rcx
	push %rax
	mov $2, %eax
	pop %rcx
	cqo
	imul %rcx
	push %rax
	mov $2, %eax
	neg %rax
	push %rax
	mov $2, %eax
	pop %rcx
	add %rcx, %rax
	pop %rcx
	xchg %rax, %rcx
	cqo
	idiv %rcx
	ret
