

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

flag_group_t flag_group1;

void task1_func(void *param)
{	

	SetSysTickPeriod(10);
	FlagGroupInit(&flag_group1, 0xFF);
	TaskDelay(3);
	FlagGroupDestroy(&flag_group1);
	while (1)
	{
		task1_flag = 0;
		TaskDelay(1);
		task1_flag = 1;
		TaskDelay(1);
		
		FlagGroupNotify(&flag_group1, 0, 0x06);
	}
}

void task2_func(void *param)
{
	uint32_t resflag;
	flag_group_info_t info;
	FlagGroupWait(&flag_group1, TFLAGGROUP_CLEAR_ALL | TFLAGGROUP_CONSUME, 0x01, &resflag, 0);
	FlagGroupGetInfo(&flag_group1, &info);
	FlagGroupWait(&flag_group1, TFLAGGROUP_CLEAR_ALL | TFLAGGROUP_CONSUME, 0x01, &resflag, 0);
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
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, &task1_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task2, task2_func, (void *)0x22222222, 1, &task2_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task3, task3_func, (void *)0x33333333, 1, &task3_env[TINY_OS_STACK_SIZE]);
	TaskInit(&task4, task4_func, (void *)0x44444444, 1, &task4_env[TINY_OS_STACK_SIZE]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[TINY_OS_STACK_SIZE]);
}


