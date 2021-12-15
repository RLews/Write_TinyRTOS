

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

typedef uint8_t (*tBlock)[100];

uint8_t mem1[20][100];
mem_block_t memblock1;

void task1_func(void *param)
{	
	uint8_t i = 0;
	tBlock block[20];
	SetSysTickPeriod(10);
	MemBlockInit(&memblock1, (uint8_t *)mem1, 100, 20);
	
	for (i = 0; i < 20; i++)
	{
		MemBlockWait(&memblock1, (uint8_t **)&block[i], 0);
	}
	TaskDelay(2);
	for (i = 0; i < 20; i++)
	{
		memset(block[i], i, 100);
		MemBlockNotify(&memblock1, (uint8_t *)block[i]);/* 先wait在Notify才能正常 */
		TaskDelay(2);
	}
	
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
	while (1)
	{
		tBlock block;
		MemBlockWait(&memblock1, (uint8_t **)&block, 0);
		task2_flag = *(uint8_t *)block;

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


