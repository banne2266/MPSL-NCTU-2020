.syntax unified
	.cpu cortex-m4
	.thumb
.data
	result: .zero 8
.text
	.global main
	.equ X, 0xFFFFFFFF
	.equ Y, 0xFFFFFFFF
	.equ Z, 0x0000FFFF
	.equ A, 0x00000000
	.equ B, 0xFFFF0000
main:
	LDR R0, =X
	LDR R1, =Y
	LDR R2, =result
	LDR R5, =Z
	LDR R7, =A
	LDR R8, =A
	LDR R9, =B
	BL kara_mul
L: B L
kara_mul:
	ANDS R3, R0, R5 // R3 Rx
	LSR R0, R0, #16 // R0 Lx
	ANDS R4, R1, R5 // R4 Ry
	LSR R1, R1, #16 // R1 Ly
	// (XL+XR)(YL+YR)
	ADDS R5, R0, R3
	ADDS R6, R1, R4
	UMULL R8, R7, R5, R6
	// (XLYL + XRYR)
	MUL R5, R0, R1
	MUL R6, R3, R4
	ADDS R6, R5, R6
	LDR R5, =A
	ADC R5, R5, #0
	// R7R8 - R5R6
	SUBS R8, R8, R6
	SBC  R7, R7, R5
	// *2^16
	ANDS R10, R8, R9
	LSL  R8, R8, #16
	LSL  R7, R7, #16
	LSR  R10, R10, #16
	ADDS R7, R7, R10
	//XRYR
	MUL R6, R3, R4
	ADDS R8, R8, R6
	ADC  R7, R7, #0
	MUL R5, R0, R1
	ADDS R7, R7, R5
	// [R7 R8]
	STRD R7, R8, [R2]
	BX LR
