
#include "stm32f10x.h"
#include "core_cm3.h"
#include "tinyOS.h"


void delay(uint32_t cnt)
{
	while (cnt > 0)
	{
		cnt--;
	}
}

int main(void)
{
	AppInit();
	
	next_task = TaskHighestReady();
	
	TaskRunFirst();
	
	return 0;
	
}





