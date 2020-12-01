	.syntax unified
	.cpu cortex-m4
	.thumb
.data
	user_stack_bottom: .zero 128
	infix_expr: .asciz "[[{{{{[]}}[][]}}]]"//{-99+ [ 10 + 20-0] }
.text
	.global main
	//move infix_expr here. Please refer to the question below.
//STMFD PUSH
//LDMFD POP
main:
	BL stack_init
	LDR R0, =infix_expr
	BL pare_check
L: B L

stack_init:
	//TODO: Setup the stack pointer(sp) to user_stack.
	//LDR SP, =user_stack_bottom
	LDR R0, =user_stack_bottom
	ADD R0, #128
	MSR MSP, R0
	BX LR

pare_check:
	//TODO: check parentheses balance, and set the error code to R0.
	MRS R4, MSP
LOOP:
	LDRB R1, [R0]	//CHECK '/0'
	CMP R1, #0
	BEQ END_LOOP

	CMP R1, #91  //CHECK '['
	BNE B1
	PUSH {R1}
	B B4
B1:
	CMP R1, #123 //CHECK '{'
	BNE B2
	PUSH {R1}
	B B4
B2:
	CMP R1, #93  //CHECK ']'
	BNE B3
	POP {R3}
	CMP R3, #91
	BNE FAIL
	B B4
B3:
	CMP R1, #125 //CHECK '}'
	BNE B4
	POP {R3}
	CMP R3, #123
	BNE FAIL
	B B4
B4:
	ADD R0, R0, #1
	B LOOP
END_LOOP:
	CMP SP, R4
	BNE FAIL
	LDR R0, =0
PARE_CHECK_RETURN:
	BX LR

FAIL:
	LDR R0, =-1
	B PARE_CHECK_RETURN
