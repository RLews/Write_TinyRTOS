
#include "stm32f10x.h"
#include "core_cm3.h"
#include "tinyOS.h"

task_t *current_task;
task_t *next_task;
//task_t *task_table[2];


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

bitmap_t task_prio_bitmap;
task_t * task_table[TINY_OS_PRO_COUNT];

//
uint8_t schedLockCount = 0;

uint32_t g_cnt = 0;

uint8_t g_share_count = 0;

void delay(uint32_t cnt);
void TaskInit(task_t *task, void (*entry)(void *), void *param, uint32_t prio, TaskStack_t * stack);
void SetSysTickPeriod(uint32_t ms);
void TaskDelay(uint32_t dly);
task_t * TaskHighestReady(void);

void TaskSched(void);




void TaskSchedInit(void)
{
	schedLockCount = 0;
	BitmapInit(&task_prio_bitmap);
}

void TaskSchedDisable(void)
{
	uint32_t sta = TaskEnterCritical();
	
	if (schedLockCount < 255)
	{
		schedLockCount++;
	}
	
	TaskExitCritical(sta);
}

void TaskSchedEnable(void)
{
	uint32_t sta = TaskEnterCritical();
	
	if (schedLockCount > 0)
	{
		schedLockCount--;
		if (schedLockCount == 0)
		{
			TaskSched();
		}
	}
	
	TaskExitCritical(sta);
}

void TaskSched(void)
{
	uint32_t status = TaskEnterCritical();
	task_t *temp;
	
	if (schedLockCount > 0)
	{
		TaskExitCritical(status);
		return;
	}
	
	temp = TaskHighestReady();
	if (temp != current_task)
	{
		next_task = temp;
		TaskSwitch();
	}

	TaskExitCritical(status);
}

void TaskInit(task_t *task, void (*entry)(void *), void *param, uint32_t prio, TaskStack_t * stack)
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
	task->prio = prio;
	task_table[prio] = task;
	BitmapSet(&task_prio_bitmap, prio);
}

task_t * TaskHighestReady(void)
{
	uint32_t highest = BitmapGetFristSet(&task_prio_bitmap);
	return task_table[highest];
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
	uint32_t status = TaskEnterCritical();
	current_task->delay_ticks = dly;
	BitmapClear(&task_prio_bitmap, current_task->prio);
	TaskExitCritical(status);
	TaskSched();
}

void TaskSystemTickHandler(void)
{
	uint8_t i = 0;
	uint32_t status = TaskEnterCritical();
	
	for (i = 0; i < TINY_OS_PRO_COUNT; i++)
	{
		if (task_table[i]->delay_ticks > 0)
		{
			task_table[i]->delay_ticks--;
		}
		else
		{
			BitmapSet(&task_prio_bitmap, i);
		}
	}
	g_cnt++;
	TaskExitCritical(status);
	
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
	TaskSchedInit();
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, &task1_env[1024]);
	TaskInit(&task2, task2_func, (void *)0x22222222, 1, &task2_env[1024]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[1024]);
	idle_task = &task_idle;
	
	next_task = TaskHighestReady();
	
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





