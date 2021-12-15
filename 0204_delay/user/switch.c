

#include "tinyOS.h"
#include "stm32f10x.h"
#include "core_cm3.h"

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET		0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI		0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)


void trigger_pendsvc(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

__asm void PendSV_Handler(void)
{
	IMPORT current_task
	IMPORT next_task
	
	MRS R0, PSP
	CBZ R0, PendSVHander_nosave
	
	STMDB R0!, {R4-R11}
	LDR R1, =current_task
	LDR R1, [R1]
	STR R0, [R1]
	
PendSVHander_nosave
	LDR	R0, =current_task
	LDR R1, =next_task
	LDR R2, [R1]
	STR R2, [R0]
	
	LDR R0, [R2]
	LDMIA R0!, {R4-R11}
	
	MSR PSP, R0
	ORR LR, LR, #0x04
	BX LR
	
#if 0
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
#endif
}


void TaskRunFirst(void)
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void TaskSwitch(void)
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}
	
	
