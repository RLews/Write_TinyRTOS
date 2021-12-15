

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

void TaskSchedRemove(task_t * task)
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

void TaskInit(task_t *task, void (*entry)(void *), void *param, uint32_t prio, TaskStack_t * stack, uint32_t size)
{
	uint32_t *stacktop;
	
	task->stackbase = stack;
	task->stacksize = size;
	memset(stack, 0, size);
	stacktop = stack + size / sizeof(TaskStack_t);
	
	*(--stacktop) = (unsigned long)(1 << 24);
	*(--stacktop) = (unsigned long)entry;
	*(--stacktop) = (unsigned long)0x14;
	*(--stacktop) = (unsigned long)0x12;
	*(--stacktop) = (unsigned long)0x03;
	*(--stacktop) = (unsigned long)0x02;
	*(--stacktop) = (unsigned long)0x01;
	*(--stacktop) = (unsigned long)param;
	*(--stacktop) = (unsigned long)0x11;
	*(--stacktop) = (unsigned long)0x10;
	*(--stacktop) = (unsigned long)0x09;
	*(--stacktop) = (unsigned long)0x08;
	*(--stacktop) = (unsigned long)0x07;
	*(--stacktop) = (unsigned long)0x06;
	*(--stacktop) = (unsigned long)0x05;
	*(--stacktop) = (unsigned long)0x04;
	
	task->slice = TINY_OS_SLICE_MAX;
 	task->stack = stacktop;
	task->delay_ticks = 0;
	task->prio = prio;
	task->state = TINYOS_TASK_STATE_READY;
	task->suspend_cnt = 0;
	
	task->clean = (void (*)(void *))0;
	task->clean_param = (void *)0;
	task->request_delflag = 0;
	
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

void TaskSetCleanCallBack(task_t *task, void (*clean)(void *param), void * param)
{
	task->clean = clean;
	task->clean_param = param;
	
}

void TaskForceDelete(task_t * task)
{
	uint32_t status = TaskEnterCritical();
	
	if (task->state & TINYOS_TASK_STATE_DELAYED)
	{
		TimeTaskRemove(task);
	}
	else if ( !(task->state & TINYOS_TASK_STATE_SUSPEND) )
	{
		TaskSchedRemove(task);
	}
	else
	{}
	
	if (task->clean)
	{
		task->clean(task->clean_param);
	}
	
	if (current_task == task)
	{
		TaskSched();
	}
	
	TaskExitCritical(status);
}

void TaskRequestDelete(task_t *task)
{
	uint32_t status = TaskEnterCritical();
	task->request_delflag = 1;
	TaskExitCritical(status);
}

uint8_t TaskIsRequestDeleted(void)
{
	uint8_t del = 0;
	uint32_t status = TaskEnterCritical();
	del = current_task->request_delflag;
	TaskExitCritical(status);
	return del;
}

void TaskDeleteSelf(void)
{
	uint32_t status = TaskEnterCritical();
	
	TaskSchedRemove(current_task);
	
	if (current_task->clean)
	{
		current_task->clean(current_task->clean_param);
	}
	TaskSched();
	
	TaskExitCritical(status);
}

void TaskGetInfo(task_t *task, task_infor_t *info)
{
	uint32_t status = TaskEnterCritical();
	uint32_t *stackend;
	
	info->delay_ticks = task->delay_ticks;
	info->prio = task->prio;
	info->slice = task->slice;
	info->state = task->state;
	info->suspend_cnt = task->suspend_cnt;
	info->stacksize = task->stacksize;
	info->stackfree = 0;
	stackend = task->stackbase;
	while ( (*stackend++ == 0) && (stackend <= task->stackbase + task->stacksize/sizeof(TaskStack_t)) )
	{
		info->stackfree++;
	}
	info->stackfree *= sizeof(TaskStack_t);
	
	TaskExitCritical(status);
}



