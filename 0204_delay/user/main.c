
#include "stm32f10x.h"
#include "core_cm3.h"
#include "tinyOS.h"

task_t *current_task;
task_t *next_task;
task_t *task_table[2];


typedef struct _BLOCK_TYPE_T
{
	unsigned long * stack_ptr;
}BlockType_t;

uint8_t flag = 0;
unsigned long stack[1024] = {0};
BlockType_t *block_ptr;
BlockType_t block;


task_t task1;
task_t task2;

uint8_t task1_flag = 0;
uint8_t task2_flag = 0;

TaskStack_t task1_env[1024] = {0};
TaskStack_t task2_env[1024] = {0};

/* idle task */
task_t task_idle;
task_t * idle_task;
TaskStack_t idle_stack[1024] = {0};



void delay(uint32_t cnt);
void TaskInit(task_t *task, void (*entry)(void *), void *param, TaskStack_t * stack);
void SetSysTickPeriod(uint32_t ms);
void TaskDelay(uint32_t dly);


void TaskSched(void)
{
	if (current_task == idle_task)
	{
		if (task_table[0]->delay_ticks == 0)
		{
			next_task = task_table[0];
		}
		else if (task_table[1]->delay_ticks == 0)
		{
			next_task = task_table[1];
		}
		else
		{
			return;
		}
	}
	else
	{
		if (current_task == task_table[0])
		{
			if (task_table[1]->delay_ticks == 0)
			{
				next_task = task_table[1];
			}
			else if (current_task->delay_ticks != 0)
			{
				next_task = idle_task;
			}
			else
			{
				return;
			}
		}
		else if (current_task == task_table[1])
		{
			if (task_table[0]->delay_ticks == 0)
			{
				next_task = task_table[0];
			}
			else if (current_task->delay_ticks != 0)
			{
				next_task = idle_task;
			}
			else
			{
				return;
			}
		}
	}
	
#if 0
	if (current_task == task_table[0])
	{
		next_task = task_table[1];
	}
	else
	{
		next_task = task_table[0];	
	}
#endif
	TaskSwitch();
}

void TaskInit(task_t *task, void (*entry)(void *), void *param, TaskStack_t * stack)
{
	*(--stack) = (unsigned long)(1 << 24);
	*(--stack) = (unsigned long)entry;
	*(--stack) = (unsigned long)0x14;
	*(--stack) = (unsigned long)0x12;
	*(--stack) = (unsigned long)0x03;
	*(--stack) = (unsigned long)0x02;
	*(--stack) = (unsigned long)0x01;
	*(--stack) = (unsigned long)param;
	*(--stack) = (unsigned long)0x11;
	*(--stack) = (unsigned long)0x10;
	*(--stack) = (unsigned long)0x09;
	*(--stack) = (unsigned long)0x08;
	*(--stack) = (unsigned long)0x07;
	*(--stack) = (unsigned long)0x06;
	*(--stack) = (unsigned long)0x05;
	*(--stack) = (unsigned long)0x04;
	
	task->stack = stack;
	task->delay_ticks = 0;
}

void task1_func(void *param)
{
	SetSysTickPeriod(10);
	
	while (1)
	{
		task1_flag = 0;
		TaskDelay(5);
		task1_flag = 1;
		TaskDelay(5);
		
		//TaskSched();
	}
}

void task2_func(void *param)
{
	while (1)
	{
		task2_flag = 0;
		TaskDelay(10);
		task2_flag = 1;
		TaskDelay(10);
		
		//TaskSched();
	}
}


void idleTask(void * param)
{
	while (1)
	{
		
	}
}

void delay(uint32_t cnt)
{
	while (cnt > 0)
	{
		cnt--;
	}
}

void TaskDelay(uint32_t dly)
{
	current_task->delay_ticks = dly;
	TaskSched();
}

void TaskSystemTickHandler(void)
{
	uint8_t i = 0;
	
	for (i = 0; i < 2; i++)
	{
		if (task_table[i]->delay_ticks > 0)
		{
			task_table[i]->delay_ticks--;
		}
	}
	
	TaskSched();
}

void SetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock /1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void)
{
	TaskSystemTickHandler();
}

int main(void)
{
	TaskInit(&task1, task1_func, (void *)0x11111111, &task1_env[1024]);
	TaskInit(&task2, task2_func, (void *)0x22222222, &task2_env[1024]);
	
	TaskInit(&task_idle, idleTask, (void *)0, &idle_stack[1024]);
	idle_task = &task_idle;
	
	task_table[0] = &task1;
	task_table[1] = &task2;
	next_task = task_table[0];
	
	TaskRunFirst();
	
	return 0;

#if 0	
	while (1)
	{
	
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);		
		trigger_pendsvc();

	}
#endif
}





