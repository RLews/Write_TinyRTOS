

#include "tinyOS.h"

list_t task_delayqueue;

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

void TaskDelay(uint32_t dly)
{
	uint32_t status = TaskEnterCritical();
	
	TimeTaskWait(current_task, dly);
	TaskSchedUnRdy(current_task);
	TaskExitCritical(status);
	TaskSched();
}

