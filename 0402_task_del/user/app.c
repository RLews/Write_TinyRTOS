

#include "tinyOS.h"


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

void task1Destory(void * param)
{
	task1_flag = 0;
}

void task1_func(void *param)
{	
	
	SetSysTickPeriod(10);
	
	TaskSetCleanCallBack(current_task, task1Destory, (void *)0);
	
	while (1)
	{
		task1_flag = 0;
		TaskDelay(1);
		task1_flag = 1;
		TaskDelay(1);
	}
}

void task2_func(void *param)
{
	uint8_t task1_del = 0;
	while (1)
	{
		task2_flag = 0;
		TaskDelay(1);
		task2_flag = 1;
		TaskDelay(1);
		if (task1_del == 0)
		{
			task1_del = 1;
			TaskForceDelete(&task1);
		}
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
		if (TaskIsRequestDeleted())
		{
			task3_flag = 0;
			TaskDeleteSelf();
		}
	}
}

void task4_func(void *param)
{
	uint8_t del = 0;
	while (1)
	{
		task4_flag = 0;
		TaskDelay(1);
		task4_flag = 1;
		TaskDelay(1);
		if (del == 0)
		{
			del = 1;
			TaskRequestDelete(&task3);
		}
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
	TaskInit(&task3, task3_func, (void *)0x33333333, 0, &task3_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task4, task4_func, (void *)0x44444444, 1, &task4_env[TINY_OS_STACK_SIZE]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[TINY_OS_STACK_SIZE]);
}


