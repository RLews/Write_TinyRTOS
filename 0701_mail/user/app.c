

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

mbox_t mbox1;
mbox_t mbox2;
void *mbox1buf[20];
void *mbox2buf[20];
uint32_t msg1[20];
uint32_t msg2[20];
uint32_t msg[20];


void task1_func(void *param)
{	
	SetSysTickPeriod(10);
	MboxInit(&mbox1, mbox1buf, 20);
	MboxInit(&mbox2, mbox2buf, 20);
	MboxWait(&mbox1, (void *)msg, 0);
	
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
	uint8_t flag = 0;
	while (1)
	{
		task2_flag = 0;
		TaskDelay(1);
		task2_flag = 1;
		TaskDelay(1);
		if (flag == 0)
		{
			flag = 1;
			MboxDestroy(&mbox1);
		}

	}
}

void task3_func(void *param)
{	
	while (1)
	{
		uint8_t i = 0;
		for (i = 0; i < 20; i++)
		{
			msg2[i] = i;
			MboxNotify(&mbox2, &msg2[i], MBOX_SEND_NORMAL);
		}
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
		uint32_t *msg;
		MboxWait(&mbox2, (void *)msg, 0);
		task4_flag = *msg;
		MboxFlush(&mbox2);

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


