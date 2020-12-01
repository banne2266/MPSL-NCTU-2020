.data
	arr1: .byte 0x19, 0x34, 0x14, 0x32, 0x52, 0x23, 0x61, 0x29
	arr2: .byte 0x18, 0x17, 0x33, 0x16, 0xFA, 0x20, 0x55, 0xAC

.text
	.global main
	.equ SIZE, 8

do_sort:
	movs R1, #0//index i of loop1
loop1:
	movs R2, #0//index j of loop2
loop2:
	add R4, R0, R2//R4 = arr+j
	add R5, R4, #1//R5 = arr+j+1
	ldrb R6, [R4]//R6 = arr[j]
	ldrb R7, [R5]//R7 = arr[j+1]
	cmp R6, R7
	ble end_if

	movs R3, R6//if(arr[j]<arr[j+1]) swap
	movs R6, R7
	movs R7, R3

	strb R6, [R4]
	strb R7, [R5]

end_if:
	add R2, #1//j++
	add R3, R1, R2//i+j
	cmp R3, #SIZE-1//i+j<SIZE-1
	blt loop2//end of loop2

	add R1, #1//i++
	cmp R1, #SIZE//i<SIZE
	blt loop1//end of loop1

	bx lr

main:
	ldr R0, =arr1
	bl do_sort
	ldr R0, =arr2
	bl do_sort
L: b L
