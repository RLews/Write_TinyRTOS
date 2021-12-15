

__asm void PendSV_Handler(void)
{
	IMPORT block_ptr
	
	LDR	R0, =block_ptr
	LDR	R0, [R0]
	LDR R0, [R0]
	
	STMDB R0!, {R4-R11} //save
	LDR R1, =block_ptr
	LDR R1, [R1]
	STR R0, [R1]
	
	ADD R4, R4, #1
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11} //recover
	
	BX LR	//return
}
