.text
	.global main
	.equ N, 39

fib://TODO
	cmp R0, #100
	bgt n_out_of_range
	cmp R0, #0
	blt n_out_of_range

	cmp R0, #2
	blt n_eq_0_or_1

	movs R1, #2//R1 is the index
	movs R2, #0//R2 is fib(n-2)
	movs R3, #1//R3 is fib(n-1)
loop:
	add R4, R2, R3
	bvs fib_overflow
	cmp R1, R0
	beq fib_return
	movs R2, R3
	movs R3, R4
	add R1, #1
	b loop

fib_overflow:
	ldr R4, =0x-2
	b fib_return

fib_return:
	bx lr

n_out_of_range:
	ldr R4, =0x-1
	b fib_return

n_eq_0_or_1:
	movs R4, R0
	b fib_return


main:
	ldr R0, =N
	bl fib
L: b L

