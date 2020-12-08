	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	//TODO: put 0 to F 7-Seg LED pattern here
	arr: .byte 0B1111110, 0B0110000, 0B1101101, 0B1111001, 0B0110011, 0B1011011, 0B1011111, 0B1110000, 0B1111111, 0B1111011, 0B1110111, 0B0011111, 0B1001110, 0B0111101, 0B1001111, 0B1000111

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
loop:
	BL DisplayDigit
	B loop

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
DisplayDigit:
//TODO: Display 0 to F at first digit on 7-SEG LED.
	PUSH {LR}
	MOVS R4, #0
	LDR R5, =arr
DD_LOOP:
	MOVS R0, #1
	LDRB R1, [R5, R4]
	BL MAX7219Send
	BL Delay

	ADDS R4, #1
	CMP R4, #16
	BLT DD_LOOP

	POP {LR}
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
//set decode mode to no decode
	MOVS R0, #0X09
	MOVS R1, #0X00
	BL MAX7219Send
//set intensity to max on
	MOVS R0, #0X0A
	MOVS R1, #0X0F
	BL MAX7219Send
//set scan limit to 0 only
	MOVS R0, #0X0B
	MOVS R1, #0X00
	BL MAX7219Send
//set display trst to normal mode
	MOVS R0, #0X0F
	MOVS R1, #0X00
	BL MAX7219Send

	POP {LR}
	BX LR


Delay:
	/* TODO: Write a delay 1 sec function */
	// You can implement this part by busy waiting.
	PUSH {R6, R7}
	MOVS R6, #0
DELAY_LOOP:
	ADDS R6, #1
	LDR R7, =#650000
	CMP R6, R7
	BLT DELAY_LOOP
	// Timer and Interrupt will be introduced in later lectures.
	POP {R6, R7}
	BX LR
