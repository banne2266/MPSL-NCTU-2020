	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global CheckPress
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OTYPER, 0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_ODR, 0x48000814
	.equ GPIOC_IDR, 0x48000810

CheckPress:
	PUSH {R4-R8}
	LDR R0, =GPIOC_IDR
	MOVS R3, #0
	MOVS R4, #0
P_LOOP:
	LDR R2, [R0]
	LSR R2, #13
	EOR R2, #1
	AND R2, #1
	ADDS R4, R2
	CMP R2, #0
	BNE P_LOOP

	LDR R5, =#4000
	CMP R4, R5
	BLT P_OUT
	MOVS R3, #1

	LDR R5, =#130000
	CMP R4, R5
	BLT P_OUT
	MOVS R3, #2
P_OUT:
	MOVS R0, R3
	POP {R4-R8}
	BX LR