	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	password:	.byte 0b0101

.text
	.global main
	.equ RCC_AHB2ENR, 0x4002104C
	.equ GPIOA_MODER, 0x48000000
	.equ GPIOA_OTYPER, 0x48000004
	.equ GPIOA_OSPEEDR, 0x48000008
	.equ GPIOA_PUPDR, 0x4800000C
	.equ GPIOA_ODR, 0x48000014
	.equ GPIOC_MODER, 0x48000800
	.equ GPIOC_OTYPER, 0x48000804
	.equ GPIOC_OSPEEDR, 0x48000808
	.equ GPIOC_PUPDR, 0x4800080C
	.equ GPIOC_ODR, 0x48000814
	.equ GPIOC_IDR, 0x48000810

main:
	BL GPIO_init
	//(option) Test! BLINK LED 2 TIMES
	MOVS R3, #2
	BL BLINK

	MOVS R0, #0//R0 AS TIMER, R1 AS ADDR, R2 AS LIGHT, R3 AS BUTTON
	MOVS R3, #0
Loop:
	/* TODO: Check the button status to determine whether to
	pause updating the LED pattern*/

	BL CheckPress
	CMP R3, #1
	BNE Loop
	BL READ_DIP
	BL COMPARE
	BL BLINK
	B Loop


BLINK://R3 STORE BLINK TIMES
	PUSH {LR}
	LDR R1, =GPIOC_ODR
BLINK_LOOP:
	MOVS R0, #(1<<4)
	STRH R0, [R1]
	BL Delay
	MOVS R0, #0
	STRH R0, [R1]
	BL Delay
	SUBS R3, #1
	CMP R3, #0
	BNE BLINK_LOOP

	POP {LR}
	BX LR


COMPARE://R2 STORE DIP INPUT, R3 STORE RETURN VALUE
	LDR R0, =password
	LDRB R2, [R0]
	AND R2, #0X000000F
	CMP R1, R2
	BNE CMP_N
	MOVS R3, #3
	BX LR
CMP_N:
	MOVS R3, #1
	BX LR


READ_DIP:
	LDR R0, =GPIOC_IDR
	LDR R1, [R0]
	EOR R1, 0XFFFFFFFF
	AND R1, #0X000000F
	BX LR


CheckPress:
	/* TODO: Do debounce and check button state */
	//IF PRESS R3 = 1

	LDR R0, =GPIOC_IDR
	LDR R1, [R0]
	//AND R1, R1, #(1<<13)
	LSR R1, #13

	MOVS R6, #0
Press_LOOP:
	ADDS R6, #1
	LDR R7, =#40000
	CMP R6, R7
	BLT Press_LOOP

	LDR R2, [R0]
	LSR R2, #13
	EOR R1, R1, R2		//=0 IF R1 == R2

	//AND R2, R2, #(1<<13)
	EOR R2, R2, #1
	ORR R1, R2//R1==R2 && R2==(1<<13)

	CMP R1, #0
	BEQ Press_OUT
	MOVS R3, #1
Press_OUT:
	BX LR



GPIO_init:
	/* TODO: Initialize LED, button GPIO pins */
	// Enable AHB2 clock
	MOVS R0, #0b101
	LDR R1, =RCC_AHB2ENR
	STR R0, [R1]

	/* Set LED gpio output */
	// Set gpio pins as output mode
	LDR R0, =#0b010100000000
	LDR R1, =GPIOC_MODER
	LDR R2, [R1]
	LDR R3, =#0XFFFFF000
	//MOVS R3, 0XFFFFFF00
	AND R2, R3
	ORR R2, R2, R0
	STR R2, [R1]
	// Keep PUPDR as the default value(pull-up)
	// Set output speed register
	//MOVS R0, #0b10101010
	LDR R0, =0b1010101010
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
	AND R2, #0xF3FFFFFF
	ORR R2, R2, R0
	STR R2, [R1]

	BX LR


Delay:
	/* TODO: Write a delay 1 sec function */
	// You can implement this part by busy waiting.
	MOVS R6, #0
DELAY_LOOP:
	ADDS R6, #1
	LDR R7, =#300000
	CMP R6, R7
	BLT DELAY_LOOP
	// Timer and Interrupt will be introduced in later lectures.
	BX LR
