

#include "tinyOS.h"

#if TINYOS_ENABLE_SEM

void SemInit(sem_t *sem, uint32_t start_cnt, uint32_t max)
{
	EventInit(&sem->event, EVENT_TYPE_SEM);
	sem->max = max;
	if (sem->count == 0)
	{
		sem->count = start_cnt;
	}
	else
	{
		sem->count = (start_cnt > max) ? (max) : (start_cnt);
	}
}

uint32_t SemWait(sem_t *sem, uint32_t wait_ticks)
{
	uint32_t sta = TaskEnterCritical();
	
	if (sem->count > 0)
	{
		sem->count--;
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		EventWait(&sem->event, current_task, (void*)0, EVENT_TYPE_SEM, wait_ticks);
		TaskExitCritical(sta);
		TaskSched();
		return current_task->wait_event_result;
	}
}

uint32_t SemNoWait(sem_t *sem)
{
	uint32_t sta = TaskEnterCritical();
	
	if (sem->count > 0)
	{
		sem->count--;
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		TaskExitCritical(sta);
		return ERROR_RESOURCE_UNAVALIABLE;
	}
}

void SemNotify(sem_t *sem)
{
	uint32_t sta = TaskEnterCritical();
	
	if (EventWaitCount(&sem->event) > 0)
	{
		task_t *task = EventWakeUp(&sem->event, (void *)0, ERROR_NO_ERROR);
		if (task->prio < current_task->prio)
		{
			TaskSched();
		}
	}
	else
	{
		sem->count++;
		if ( (sem->max != 0) && (sem->count > sem->max) )
		{
			sem->count = sem->max;
		}
	}
	TaskExitCritical(sta);
}

void SemGetInfo(sem_t *sem, sem_info_t *info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->count = sem->count;
	info->max = sem->max;
	info->task_cnt = EventWaitCount(&sem->event);
	
	TaskExitCritical(sta);
}	


uint32_t SemDestory(sem_t *sem)
{
	uint32_t sta = TaskEnterCritical();
	uint32_t count = EventRemoveAll(&sem->event, (void*)0, ERROR_DEL);
	sem->count = 0;
	
	TaskExitCritical(sta);
	
	if (count > 0)
	{
		TaskSched();
	}
	
	return count;
}


#endif






