

#include "tinyOS.h"


void EventInit(event_t * event, event_type_t type)
{
	event->type = EVENT_TYPE_UNKONW;
	ListInit(&event->wait_list);
}


void EventWait(event_t *event, task_t *task, void * msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = TaskEnterCritical();
	
	task->state |= state;
	task->wait_event = event;
	task->event_msg = msg;
	task->wait_event_result = ERROR_NO_ERROR;
	TaskSchedUnRdy(task);
	ListAddLast(&event->wait_list, &task->link_node);
	
	if (timeout)
	{
		TimeTaskWait(task, timeout);
	}
	
	TaskExitCritical(status);
}

task_t * EventWakeUpTask(event_t *event, task_t *task, void *msg, uint32_t res)
{
	uint32_t status = TaskEnterCritical();
	
	ListRemove(&event->wait_list, &task->link_node);

	task->wait_event = (event_t *)0;
	task->event_msg = msg;
	task->wait_event_result = res;
	task->state &= ~TINYOS_TASK_WAIT_MASK;
		
	if (task->delay_ticks != 0)
	{
		TimeWakeUp(task);
	}
		
	TaskSchedRdy(task);
	
	TaskExitCritical(status);
	
	return task;
}

task_t * EventWakeUp(event_t *event, void *msg, uint32_t res)
{
	node_t *node;
	task_t *task = (task_t *)0;
	uint32_t status = TaskEnterCritical();
	
	if ( (node = ListRemoveFirst(&event->wait_list)) != (node_t *)0 )
	{
		task = (task_t *)NODE_PARENT(node, task_t, link_node);
		task->wait_event = (event_t *)0;
		task->event_msg = msg;
		task->wait_event_result = res;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		
		if (task->delay_ticks != 0)
		{
			TimeWakeUp(task);
		}
		
		TaskSchedRdy(task);
	}
	
	TaskExitCritical(status);
	
	return task;
}

void EventRemoveTask(task_t *task, void *msg, uint32_t res)
{
	uint32_t status = TaskEnterCritical();
	
	ListRemove(&task->wait_event->wait_list, &task->link_node);
	task->wait_event = (event_t *)0;
	task->event_msg = msg;
	task->wait_event_result = res;
	task->state &= ~TINYOS_TASK_WAIT_MASK;
	
	TaskExitCritical(status);
}

uint32_t EventRemoveAll(event_t *event, void *msg, uint32_t res)
{
	uint32_t count = 0;
	node_t *node;
	uint32_t status = TaskEnterCritical();
	
	count = ListCount(&event->wait_list);
	while ( (node = ListRemoveFirst(&event->wait_list)) != (node_t *)0 )
	{
		task_t *task = (task_t *)NODE_PARENT(node, task_t, link_node);
		task->wait_event = (event_t *)0;
		task->event_msg = msg;
		task->wait_event_result = res;
		task->state &= ~TINYOS_TASK_WAIT_MASK;
		if (task->delay_ticks != 0)
		{
			TimeWakeUp(task);
		}
		TaskSchedRdy(task);
	}
		
	TaskExitCritical(status);
	
	return count;
}

uint32_t EventWaitCount(event_t *event)
{
	uint32_t count = 0;
	uint32_t status = TaskEnterCritical();
	
	count = ListCount(&event->wait_list);
	
	TaskExitCritical(status);
	
	return count;
}


