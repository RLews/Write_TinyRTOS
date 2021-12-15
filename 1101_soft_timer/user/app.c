

#include "tinyOS.h"
#include <string.h>


static task_t task1;
static task_t task2;
static task_t task3;
static task_t task4;

static TaskStack_t task1_env[TINY_OS_STACK_SIZE] = {0};
static TaskStack_t task2_env[TINY_OS_STACK_SIZE] = {0};
static TaskStack_t task3_env[TINY_OS_STACK_SIZE] = {0};
static TaskStack_t task4_env[TINY_OS_STACK_SIZE] = {0};

/* idle task */
static task_t task_idle;
static TaskStack_t idle_stack[TINY_OS_STACK_SIZE] = {0};

uint8_t task1_flag = 0;
uint8_t task2_flag = 0;
uint8_t task3_flag = 0;
uint8_t task4_flag = 0;

stimer_t timer1;
stimer_t timer2;
stimer_t timer3;

uint32_t bit1 = 0;
uint32_t bit2 = 0;
uint32_t bit3 = 0;


void timerfunc(void * arg)
{
	uint32_t * ptr = (uint32_t *)arg;
	
	*ptr ^= 0x01;
}

void task1_func(void *param)
{	
	uint8_t stop = 0;
	SetSysTickPeriod(10);
	StimerInit(&timer1, 50, 10, timerfunc, (void *)&bit1, STIMER_CONFIG_TYPE_HARD);
	StimerStart(&timer1);
	StimerInit(&timer2, 200, 20, timerfunc, (void *)&bit2, STIMER_CONFIG_TYPE_HARD);
	StimerStart(&timer2);
	StimerInit(&timer3, 300, 00, timerfunc, (void *)&bit3, STIMER_CONFIG_TYPE_HARD);
	StimerStart(&timer3);
	while (1)
	{
		
		task1_flag = 0;
		TaskDelay(1);
		task1_flag = 1;
		TaskDelay(1);
		
		if (stop == 0)
		{
			TaskDelay(100); 
			StimerStop(&timer1);
			stop = 1;
		}
	}
}

void task2_func(void *param)
{
	while (1)
	{
		
		task2_flag = 0;
		TaskDelay(1);
		task2_flag = 1;
		TaskDelay(1);
	}
}

void task3_func(void *param)
{	
	while (1)
	{
		task3_flag = 0;
		TaskDelay(1);
		task3_flag = 1;
		TaskDelay(1);

	}
}

void task4_func(void *param)
{
	while (1)
	{
		#if 1
		task4_flag = 0;
		TaskDelay(1);
		task4_flag = 1;
		TaskDelay(1);
		#endif
	}
}


void idleTask(void * param)
{
	while (1)
	{
		
	}
}


void AppInit(void)
{
	TaskSchedInit();
	TaskDelayedInit();
	StimerModuleInit();
	
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, &task1_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task2, task2_func, (void *)0x22222222, 2, &task2_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task3, task3_func, (void *)0x33333333, 3, &task3_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task4, task4_func, (void *)0x44444444, 4, &task4_env[TINY_OS_STACK_SIZE]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[TINY_OS_STACK_SIZE]);
}


