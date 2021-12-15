
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
task_t task3;

uint8_t task1_flag = 0;
uint8_t task2_flag = 0;
uint8_t task3_flag = 0;

TaskStack_t task1_env[1024] = {0};
TaskStack_t task2_env[1024] = {0};
TaskStack_t task3_env[1024] = {0};

/* idle task */
task_t task_idle;
task_t * idle_task;
TaskStack_t idle_stack[1024] = {0};

bitmap_t task_prio_bitmap;
list_t task_table[TINY_OS_PRO_COUNT];

//
uint8_t schedLockCount = 0;

uint32_t g_cnt = 0;

uint8_t g_share_count = 0;

list_t task_delayqueue;

void delay(uint32_t cnt);









void TaskSchedInit(void)
{
	uint8_t i = 0;
	schedLockCount = 0;
	BitmapInit(&task_prio_bitmap);
	
	for (i = 0; i < TINY_OS_PRO_COUNT; i++)
	{
		ListInit(&task_table[i]);
	}
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

void TaskSchedRdy(task_t *task)
{
	ListAddFirst(&(task_table[task->prio]),&(task->link_node));
	BitmapSet(&task_prio_bitmap, task->prio);
}

void TaskSchedUnRdy(task_t *task)
{
	ListRemove(&task_table[task->prio], &(task->link_node));
	if (ListCount(&task_table[task->prio]) == 0)
	{
		BitmapClear(&task_prio_bitmap, task->prio);
	}
	
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
	
	task->slice = TINY_OS_SLICE_MAX;
	task->stack = stack;
	task->delay_ticks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_READY;
	
	NodeInit(&(task->delay_node));
	NodeInit(&(task->link_node));
	ListAddFirst(&task_table[prio],&(task->link_node));
	
	BitmapSet(&task_prio_bitmap, prio);	
}

task_t * TaskHighestReady(void)
{
	uint32_t highest = BitmapGetFristSet(&task_prio_bitmap);
	node_t *note = ListFirst(&task_table[highest]);
	return NODE_PARENT(note, task_t, link_node);
}

void task1_func(void *param)
{	

	SetSysTickPeriod(10);
	
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
		task2_flag = 0;
		delay(0x7FFFF);
		task2_flag = 1;
		delay(0x7FFFF);
	}
}

void task3_func(void *param)
{
	while (1)
	{
		task3_flag = 0;
		delay(0x1FFFF);
		task3_flag = 1;
		delay(0x1FFFF);
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
	
	TimeTaskWait(current_task, dly);
	TaskSchedUnRdy(current_task);
	TaskExitCritical(status);
	TaskSched();
}

void TaskDelayedInit(void)
{
	ListInit(&task_delayqueue);
}

void TimeTaskWait(task_t *task, uint32_t ticks)
{
	task->delay_ticks = ticks;
	ListAddLast(&task_delayqueue, &(task->delay_node));
	task->state |= TINYOS_TASK_STATE_DELAYED;
}

void TimeWakeUp(task_t *task)
{
	ListRemove(&task_delayqueue, &(task->delay_node));
	task->state &= ~TINYOS_TASK_STATE_DELAYED;
}

void TaskSystemTickHandler(void)
{
	node_t *node;
	uint32_t status = TaskEnterCritical();
	
	for (node = task_delayqueue.head.next; node != &(task_delayqueue.head); node = node->next)
	{
		task_t *task = NODE_PARENT(node, task_t, delay_node);
		if (--task->delay_ticks == 0)
		{
			TimeWakeUp(task);
			TaskSchedRdy(task);
		}
	}
	
	if (--current_task->slice == 0)
	{
		if (ListCount(&task_table[current_task->prio]) > 0)
		{
			ListRemoveFirst(&task_table[current_task->prio]);
			ListAddLast(&task_table[current_task->prio], &(current_task->link_node));
			current_task->slice = TINY_OS_SLICE_MAX;
		}
	}
	
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
#if 0
void List_test(void)
{
	uint8_t i = 0;
	list_t list;
	node_t node[8];
	node_t sp_node;
	
	NodeInit(&sp_node);
	ListInit(&list);
	for (i = 0; i < 8; i++)
	{
		NodeInit(&node[i]);
		ListAddFirst(&list, &node[i]);
	}
	for (i = 0; i < 8; i ++)
	{
		ListRemoveFirst(&list);
	}
	for (i = 0; i < 8; i++)
	{
		NodeInit(&node[i]);
		ListAddLast(&list, &node[i]);
	}
	ListInsertAfter(&list, &node[2], &sp_node);
	ListRemove(&list, &sp_node);
	ListRemoveAll(&list);
}
#endif
int main(void)
{
	TaskSchedInit();
	TaskDelayedInit();
	TaskInit(&task1, task1_func, (void *)0x11111111, 0, &task1_env[1024]);
	TaskInit(&task2, task2_func, (void *)0x22222222, 1, &task2_env[1024]);
	TaskInit(&task3, task3_func, (void *)0x33333333, 1, &task3_env[1024]);
	
	TaskInit(&task_idle, idleTask, (void *)0, TINY_OS_PRO_COUNT-1, &idle_stack[1024]);
	idle_task = &task_idle;
	
	next_task = TaskHighestReady();
	
	TaskRunFirst();
	
	return 0;
}





