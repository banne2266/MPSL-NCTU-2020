.data
	result: .byte 0
.text
.global main
	.equ X, 0xABCD
	.equ Y, 0xEFAB


main:
	movw R0, #:lower16:X
	movt R0, #:upper16:X
	movw R1, #:lower16:Y
	movt R1, #:upper16:Y
	ldr R2, =result
	bl hamm
L: b L


hamm:
	eor R0, R0, R1  //indicate the bit difference
	movs R4, #0  //record the hamming distance
	movs R1, #0  //index for looping

loop:
	movs R5, #1  //r5 = constant 1
	and R5, R0, R5	//r5 = r0 & 5
	add R4, R4, R5  //r4 += r5
	lsr R0, R0, #1  //r0 >>= 1

	add R1, R1, #1
	cmp R1, #32
	ble loop

	str R4, [R2]
	bx lr
