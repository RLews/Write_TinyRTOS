

#include "tinyOS.h"

task_t *current_task;
task_t *next_task;

bitmap_t task_prio_bitmap;
list_t task_table[TINY_OS_PRO_COUNT];

static uint8_t schedLockCount = 0;


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



task_t * TaskHighestReady(void)
{
	uint32_t highest = BitmapGetFristSet(&task_prio_bitmap);
	node_t *note = ListFirst(&task_table[highest]);
	return NODE_PARENT(note, task_t, link_node);
}

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
	
	TaskSchedRdy(task);

}

void TaskSuspend(task_t *task)
{
	uint32_t status = TaskEnterCritical();
	
	if (!(task->state & TINYOS_TASK_STATE_DELAYED))
	{
		if (++task->suspend_cnt <= 1)
		{
			task->state |= TINYOS_TASK_STATE_SUSPEND;
			TaskSchedUnRdy(task);
			if (task == current_task)
			{
				TaskSched();
			}
		}
	}
	
	TaskExitCritical(status);
}

void TaskWakeUp(task_t *task)
{
	uint32_t status = TaskEnterCritical();
	
	if ((task->state & TINYOS_TASK_STATE_SUSPEND))
	{
		if (--task->suspend_cnt == 0)
		{
			task->state &= ~TINYOS_TASK_STATE_SUSPEND;
			TaskSchedRdy(task);
			TaskSched();
		}
	}
	
	TaskExitCritical(status);
}


