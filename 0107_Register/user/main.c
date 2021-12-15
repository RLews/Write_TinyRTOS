
#include "stm32f10x.h"

#define NVIC_INT_CTRL		0xE000ED04
#define NVIC_PENDSVSET		0x10000000
#define NVIC_SYSPRI2		0xE000ED22
#define NVIC_PENDSV_PRI		0x000000FF

#define MEM32(addr)			*(volatile unsigned long *)(addr)
#define MEM8(addr)			*(volatile unsigned char *)(addr)

typedef struct _BLOCK_TYPE_T
{
	unsigned long * stack_ptr;
}BlockType_t;

uint8_t flag = 0;
unsigned long stack[1024] = {0};
BlockType_t *block_ptr;
BlockType_t block;

void trigger_pendsvc(void)
{
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

void delay(uint32_t cnt)
{
	while (cnt > 0)
	{
		cnt--;
	}
}

int main(void)
{
	block.stack_ptr = &stack[1024];
	block_ptr = &block;
	
	while (1)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
		
		
		trigger_pendsvc();
	}
}





