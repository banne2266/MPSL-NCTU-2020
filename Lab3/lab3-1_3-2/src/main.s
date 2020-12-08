	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	leds: .byte // (or leds: .word)
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
	//(option) Test! Turn on all LEDs
	LDR R1, =GPIOC_ODR
	MOVS R0, #0b1111
	STRH R0, [R1]
	MOVS R0, #0//R0 AS TIMER, R1 AS ADDR, R2 AS LIGHT, R3 AS BUTTON
	MOVS R3, #0
Loop:
	/* TODO: Check the button status to determine whether to
	pause updating the LED pattern*/

	BL CheckPress
	CMP R3, #0
	BNE STOP
	BL DisplayLED
STOP:
	BL Delay
	B Loop


CheckPress:
	/* TODO: Do debounce and check button state */
	PUSH {R0-R2}
	LDR R0, =GPIOC_IDR
	LDR R1, [R0]

	MOVS R6, #0
Press_LOOP:
	ADDS R6, #1
	LDR R7, =#40000
	CMP R6, R7
	BLT Press_LOOP

	LDR R2, [R0]
	EOR R1, R1, R2
	AND R2, R2, #(1<<13)
	ORR R1, R2//R1==R2 && R2==(1<<13)

	CMP R1, #0
	BNE Press_OUT
	CMP R3, #0
	BEQ FAIL
	MOV R3, #0
	B Press_OUT
FAIL:
	MOV R3, #1
Press_OUT:
	POP {R0-R2}
	BX LR


DisplayLED:
	/* TODO: Display LED by leds */
	LDR R1, =GPIOC_ODR
	CMP R0, #0
	BNE P0
	MOVS R2, #1
	B LED_FIN
P0:
	CMP R0, #1
	BNE P1
	MOVS R2, #3
	B LED_FIN
P1:
	CMP R0, #4
	BGT P2
	LSL R2, #1
	B LED_FIN
P2:
	CMP R0, #7
	BGT LED_FIN
	LSR R2, #1
LED_FIN:
	EOR R2, #0XF// COMMENT THIS LINE
	STRH R2, [R1]
	EOR R2, #0XF// COMMENT THIS LINE
	ADD R0, #1
	CMP R0, #7
	BLE OUT
	MOV R0, #0
OUT:
	BX LR


GPIO_init:
	/* TODO: Initialize LED, button GPIO pins */
	// Enable AHB2 clock
	MOVS R0, #0b101
	LDR R1, =RCC_AHB2ENR
	STR R0, [R1]

	/* Set LED gpio output */
	// Set gpio pins as output mode
	//MOVS R0, #0b01010101
	LDR R0, =#0b01010101
	LDR R1, =GPIOC_MODER
	LDR R2, [R1]
	LDR R3, =#0XFFFFFF00
	//MOVS R3, 0XFFFFFF00
	AND R2, R3
	ORR R2, R2, R0
	STR R2, [R1]
	// Keep PUPDR as the default value(pull-up)
	// Set output speed register
	//MOVS R0, #0b10101010
	LDR R0, =0b10101010
	LDR R1, =GPIOC_OSPEEDR
	STRH R1, [R1]

	/* Set user button(pc13) as gpio input */
	// set PC13 as input mode
	LDR R1, =GPIOC_MODER
	LDR R0, [R1]
	AND R0, #0xF3FFFFFF
	STR R0, [R1]
	// Set PC13 as Pull-up
	MOVS R0, #0X0C000000
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
	LDR R7, =#650000
	CMP R6, R7
	BLT DELAY_LOOP
	// Timer and Interrupt will be introduced in later lectures.
	BX LR
