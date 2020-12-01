	.syntax unified
	.cpu cortex-m4
	.thumb
.data
.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OTYPER, 0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_ODR, 0x48000814
	.equ GPIOC_IDR, 0x48000810
main:
	BL GPIO_init
	BL max7219_init
	MOVS R4, #0//INDEX
Loop:
	MOVS R0, R4
	BL DISPLAY
	BL CheckPress
	CMP R3, #0
	BEQ Loop
	ADDS R4, #1
	CMP R3, #2
	BEQ RESET
	B Loop
RESET:
	MOVS R4, #0
	B Loop


DISPLAY:
	PUSH {R4-R8,LR}
	BL fib
	MOVS R4, #0//LOOP INDEX
	MOVS R7, #0//SAVE BCD
DP_LOOP:

	MOVS R5, #0
	MOVS R3, #0B1111
DP_LOOP2:
	AND R2, R3, R7 //R2=(1111<<K)&BCD
	MOVS R1, R5, LSL #2
	LSR R2, R1
	CMP R2, 5
	BLT SKIP
	ADDS R2, #3
	LSL R2, R1

	EOR R3, #0XFFFFFFFF
	AND R7, R3
	EOR R3, #0XFFFFFFFF
	ORR R7, R2//BCD[COL] += 5
SKIP:
	LSL R3, #4
	ADDS R5, #1
	CMP R5, #8
	BLT DP_LOOP2

	AND R6, R0, #(1<<31)
	LSR R6, #31
	LSL R7, #1
	ORR R7, R6
	LSL R0, #1

	ADDS R4, #1
	CMP R4, #32
	BLT DP_LOOP

	MOVS R4, #7///R4 LOOP INDEX
	MOVS R3, #0XF0000000
	MOVS R2, #0//INDEX FOR FIRST NON-0
D2_LOOP:
	AND R5, R3, R7
	MOVS R1, R4, LSL #2
	LSR R5, R1
	MOVS R0, R4
	ADDS R0, #1
	MOVS R1, R5

	CMP R2, #1
	BEQ SKIP3
	CMP R1, #0
	BNE SKIP2
	CMP R0, #1
	BEQ SKIP3
	MOVS R1, 0XF
	B SKIP3
SKIP2:
	MOVS R2, #1
SKIP3:
	BL MAX7219Send

	SUBS R4, #1
	LSR R3, #4
	CMP R4, #0
	BGE D2_LOOP


	POP {R4-R8,LR}
	BX LR


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
	POP {R4-R8}
	BX LR



fib://R0 = FIB(R0)
	PUSH {R4, R5}
	cmp R0, #2
	blt n_eq_0_or_1
	movs R1, #2//R1 is the index
	movs R2, #0//R2 is fib(n-2)
	movs R3, #1//R3 is fib(n-1)
loop:
	add R4, R2, R3
	LDR R5, =#99999999
	CMP R4, R5
	BGT fib_overflow
	cmp R1, R0
	beq fib_return
	movs R2, R3
	movs R3, R4
	add R1, #1
	b loop

n_eq_0_or_1:
	movs R4, R0
	b fib_return

fib_overflow:
	ldr R4, =#99999999
	b fib_return

fib_return:
	MOVS R0, R4
	POP {R4, R5}
	bx lr


GPIO_init:
	//TODO: Initialize GPIO pins for max7219 DIN, CS and CLK
		// Enable AHB2 clock
	MOVS R0, #0b101
	LDR R1, =RCC_AHB2ENR
	STR R0, [R1]

	/* Set LED gpio output */
	// Set gpio pins as output mode
	LDR R0, =#0b010101
	LDR R1, =GPIOC_MODER
	LDR R2, [R1]
	LDR R3, =#0XFFFFFFC0
	AND R2, R3
	ORR R2, R2, R0
	STR R2, [R1]
	// Keep PUPDR as the default value(pull-up)
	// Set output speed register
	//MOVS R0, #0b10101010
	LDR R0, =0b101010
	LDR R1, =GPIOC_OSPEEDR
	STRH R1, [R1]


	/* Set user button(pc13) as gpio input */
	// set PC13 as input mode
	LDR R1, =GPIOC_MODER
	LDR R0, [R1]
	AND R0, #0xF3FFFFFF
	STR R0, [R1]
	// Set PC13 as Pull-up
	LDR R0, =#0X0C000055
	LDR R1, =GPIOC_PUPDR
	LDR R2, [R1]
	AND R2, #0xFBFFFFFF
	ORR R2, R2, R0
	STR R2, [R1]

	BX LR

MAX7219Send:
//input parameter: r0 is ADDRESS , r1 is DATA
//TODO: Use this function to send a message to max7219
	PUSH {R4-R8}
	LSL R5, R0, #8
	ORR R5, R1
//R4 ODR, R5 DATA, R6, OUTPUT
	LDR R4, =GPIOC_ODR
	LDR R6, [R4]
	AND R6, 0XFFFFFFF8
	ORR R6, 0X0
	STR R6, [R4]

	MOVS R7, #0//R7 IS LOOP INDEX
SENDLOOP:
///SET DIN TO R5[31]
	AND R8, R5, #(1<<15)
	LSR R8, #13
	LSL R5, #1
	LDR R6, [R4]
	AND R6, 0XFFFFFFFB
	ORR R6, R8
	STR R6, [R4]
///SET CLK TO 1
	LDR R6, [R4]
	AND R6, 0XFFFFFFFE
	ORR R6, 0X1
	STR R6, [R4]
///SET CLK TO 0
	LDR R6, [R4]
	AND R6, 0XFFFFFFFE
	ORR R6, 0X0
	STR R6, [R4]

	ADDS R7, #1
	CMP R7, #16
	BLT SENDLOOP

///SET CS TO 1
	LDR R6, [R4]
	AND R6, 0XFFFFFFFD
	ORR R6, 0B10
	STR R6, [R4]
///SET CLK TO 1
	LDR R6, [R4]
	AND R6, 0XFFFFFFFE
	ORR R6, 0X0
	STR R6, [R4]

	LDR R6, [R4]
	AND R6, 0XFFFFFFF8
	ORR R6, 0X0
	STR R6, [R4]
	POP {R4-R8}
	BX LR


max7219_init:
//TODO: Initialize max7219 registers
	PUSH {LR}
//set shutdown register to normal mode
	MOVS R0, #0X0C
	MOVS R1, #0X1
	BL MAX7219Send
//set decode mode to BCD decode
	MOVS R0, #0X09
	MOVS R1, #0XFF
	BL MAX7219Send
//set intensity to max on
	MOVS R0, #0X0A
	MOVS R1, #0X0F
	BL MAX7219Send
//set scan limit to 7
	MOVS R0, #0X0B
	MOVS R1, #0X07
	BL MAX7219Send
//set display test to normal mode
	MOVS R0, #0X0F
	MOVS R1, #0X00
	BL MAX7219Send

	POP {LR}
	BX LR
