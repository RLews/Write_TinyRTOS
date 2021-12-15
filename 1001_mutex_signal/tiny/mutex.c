



#include "tinyOS.h"


void MutexInit(mutex_t *mutex)
{
	EventInit(&mutex->event, EVENT_TYPE_MUTEX);
	mutex->owner = (task_t *)0;
	mutex->lockcnt = 0;
	mutex->owner_original_prio = TINY_OS_PRO_COUNT;
}

uint32_t MutexWait(mutex_t *mutex, uint32_t ticks)
{
	uint32_t sta = TaskEnterCritical();
	
	if (mutex->lockcnt <= 0)
	{
		mutex->owner = current_task;
		mutex->owner_original_prio = current_task->prio;
		mutex->lockcnt++;
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		if (mutex->owner == current_task)
		{
			mutex->lockcnt++;
			TaskExitCritical(sta);
			return ERROR_NO_ERROR;
		}
		else
		{
			if (current_task->prio < mutex->owner->prio)
			{
				task_t *owner = mutex->owner;
				if (owner->state == TINYOS_TASK_STATE_READY)
				{
					TaskSchedUnRdy(owner);
					owner->prio = current_task->prio;
					TaskSchedRdy(owner);
				}
				else
				{
					owner->prio = current_task->prio;
				}
			}
			
			EventWait(&mutex->event, current_task, (void *)0, EVENT_TYPE_MUTEX, ticks);
			TaskExitCritical(sta);
			TaskSched();
			return current_task->wait_event_result; 
		}
	}
	
}

uint32_t MutexNoWaitGet(mutex_t *mutex)
{
	uint32_t sta = TaskEnterCritical();
	
	if (mutex->lockcnt <= 0)
	{
		mutex->owner = current_task;
		mutex->owner_original_prio = current_task->prio;
		mutex->lockcnt++;
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		if (mutex->owner == current_task)
		{
			mutex->lockcnt++;
			TaskExitCritical(sta);
			return ERROR_NO_ERROR;
		}
		TaskExitCritical(sta);
		return ERROR_RESOURCE_UNAVALIABLE;
	}
	
}

uint32_t MutexNotify(mutex_t *mutex)
{
	uint32_t sta = TaskEnterCritical();
	
	if (mutex->lockcnt <= 0)
	{
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	
	if (mutex->owner != current_task)
	{
		TaskExitCritical(sta);
		return ERROR_OWNER;
	}
	
	if (--mutex->lockcnt > 0)
	{
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	
	if (mutex->owner_original_prio != mutex->owner->prio)
	{
		if (mutex->owner->state == TINYOS_TASK_STATE_READY)
		{
			TaskSchedUnRdy(mutex->owner);
			current_task->prio = mutex->owner_original_prio;
			TaskSchedRdy(mutex->owner);
		}
		else
		{
			current_task->prio = mutex->owner_original_prio;
		}
	}
	
	if (EventWaitCount(&mutex->event) > 0)
	{
		task_t * task = EventWakeUp(&mutex->event, (void *)0, ERROR_NO_ERROR);
		
		mutex->owner = task;
		mutex->owner_original_prio = task->prio;
		mutex->lockcnt++;
		
		if (task->prio < current_task->prio)
		{
			TaskSched();
		}
	}
	
	TaskExitCritical(sta);
	return ERROR_NO_ERROR;
}


uint32_t MutexDestroy(mutex_t *mutex)
{
	uint32_t count = 0;
	uint32_t sta = TaskEnterCritical();
	
	if (mutex->lockcnt > 0)
	{
		if (mutex->owner_original_prio != mutex->owner->prio)
		{
			if (mutex->owner->state == TINYOS_TASK_STATE_READY)
			{
				TaskSchedUnRdy(mutex->owner);
				mutex->owner->prio = mutex->owner_original_prio;
				TaskSchedRdy(mutex->owner);
			}
			else
			{
				mutex->owner->prio = mutex->owner_original_prio;
			}
		}
		
		count = EventRemoveAll(&mutex->event, (void *)0, ERROR_DEL);
		if (count > 0)
		{
			TaskSched();
		}
	}
	
	TaskExitCritical(sta);
	return count;
}

void MutexGetInfo(mutex_t *mutex, mutex_info_t * info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->taskcnt = EventWaitCount(&mutex->event);
	info->ownerprio = mutex->owner_original_prio;
	
	if (mutex->owner != (task_t *)0)
	{
		info->inheritedprio = mutex->owner->prio;
	}
	else
	{
		info->inheritedprio = TINY_OS_PRO_COUNT;		
	}
	info->owner = mutex->owner;
	info->lockedcnt = mutex->lockcnt;
	
	TaskExitCritical(sta);
}









