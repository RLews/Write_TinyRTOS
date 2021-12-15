

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

task_infor_t task1info;
task_infor_t task2info;
task_infor_t task3info;
task_infor_t task4info;

float g_cpu = 0.0;


void task1_func(void *param)
{		
	uint32_t cnt = 0;
	while (1)
	{
		for (cnt = 0; cnt < 0x7FFFF; cnt++)
		{
			task1_flag = 0;
			task1_flag = 1;
		}
		
		TaskDelay(10);
		TaskGetInfo(current_task, &task1info);
		g_cpu = CpuUsageGet();
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
		TaskGetInfo(current_task, &task2info);
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
		TaskGetInfo(current_task, &task3info);
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
		TaskGetInfo(current_task, &task4info);
	}
}

void idleTask(void * param)
{
	TaskSchedDisable();
	
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, task1_env, sizeof(task1_env));
	TaskInit(&task2, task2_func, (void *)0x22222222, 1, task2_env, sizeof(task2_env));
	TaskInit(&task3, task3_func, (void *)0x33333333, 2, task3_env, sizeof(task3_env));
	TaskInit(&task4, task4_func, (void *)0x44444444, 3, task4_env, sizeof(task4_env));
	StimerInitTask();
	SetSysTickPeriod(TINY_OS_SYSTEM_TICK_MS);
	CpuUsageSyncWithSystick();
	
	while (1)
	{
		uint32_t status = TaskEnterCritical();
		CpuCountplus();
		TaskExitCritical(status);
	}
}


void AppInit(void)
{
	TaskSchedInit();
	TaskDelayedInit();
	StimerModuleInit();
	
	TimeTickCountInit();
	InitCpuUsageState();
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, idle_stack, sizeof(idle_stack));
}


