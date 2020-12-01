	.syntax unified
	.cpu cortex-m4
	.thumb

.text
	.global GPIO_init
	.global max7219_send
	.global max7219_init
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OTYPER, 0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_ODR, 0x48000814
	.equ GPIOC_IDR, 0x48000810

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

	BX LR

max7219_send:
//input parameter: r0 is ADDRESS , r1 is DATA
//TODO: Use this function to send a message to max7219
	PUSH {R4-R11, LR}
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
	POP {R4-R11, LR}
	BX LR



max7219_init:
//TODO: Initialize max7219 registers
	PUSH {LR}
//set shutdown register to normal mode
	MOVS R0, #0X0C
	MOVS R1, #0X1
	BL max7219_send
//set decode mode to CODE B decode
	MOVS R0, #0X09
	MOVS R1, #0XFF
	BL max7219_send
//set intensity to max on
	MOVS R0, #0X0A
	MOVS R1, #0X0F
	BL max7219_send
//set scan limit to 4
	MOVS R0, #0X0B
	MOVS R1, #0X07
	BL max7219_send
//set display trst to normal mode
	MOVS R0, #0X0F
	MOVS R1, #0X00
	BL max7219_send

	POP {LR}
	BX LR

