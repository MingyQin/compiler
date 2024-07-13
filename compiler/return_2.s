.text
.globl main

main:
	mov $6, %eax
	push %rax
	mov $3, %eax
	pop %rcx
	xchg %rax, %rcx
	cqo
	div %rcx
	push %rax
	mov $2, %eax
	pop %rcx
	xchg %rax, %rcx
	cqo
	div %rcx
	ret
