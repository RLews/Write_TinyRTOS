

#include "tinyOS.h"


static task_t task1;
static task_t task2;
static task_t task3;

static TaskStack_t task1_env[TINY_OS_STACK_SIZE] = {0};
static TaskStack_t task2_env[TINY_OS_STACK_SIZE] = {0};
static TaskStack_t task3_env[TINY_OS_STACK_SIZE] = {0};

/* idle task */
static task_t task_idle;
static TaskStack_t idle_stack[TINY_OS_STACK_SIZE] = {0};

uint8_t task1_flag = 0;
uint8_t task2_flag = 0;
uint8_t task3_flag = 0;

void task1_func(void *param)
{	
	
	SetSysTickPeriod(10);
	
	while (1)
	{
		task1_flag = 0;
		TaskSuspend(current_task);
		task1_flag = 1;
		TaskSuspend(current_task);
	}
}

void task2_func(void *param)
{

	while (1)
	{
		task2_flag = 0;
		TaskDelay(1);
		TaskWakeUp(&task1);
		task2_flag = 1;
		TaskDelay(1);
		TaskWakeUp(&task1);
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
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, &task1_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task2, task2_func, (void *)0x22222222, 1, &task2_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task3, task3_func, (void *)0x33333333, 1, &task3_env[TINY_OS_STACK_SIZE]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[TINY_OS_STACK_SIZE]);
}


