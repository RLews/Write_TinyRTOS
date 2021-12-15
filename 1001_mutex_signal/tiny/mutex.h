

#ifndef __MUTEX_H
#define __MUTEX_H

#include "tevent.h"

typedef struct _MUTEX_T
{
	event_t event;
	uint32_t lockcnt;
	task_t * owner;
	uint32_t owner_original_prio;
}mutex_t;

typedef struct _MUTEX_INFO_T
{
	uint32_t taskcnt;
	uint32_t ownerprio;
	uint32_t inheritedprio;
	task_t *owner;
	uint32_t lockedcnt;
}mutex_info_t;

void MutexInit(mutex_t *mutex);

uint32_t MutexNotify(mutex_t *mutex);

uint32_t MutexNoWaitGet(mutex_t *mutex);

uint32_t MutexWait(mutex_t *mutex, uint32_t ticks);

uint32_t MutexDestroy(mutex_t *mutex);

void MutexGetInfo(mutex_t *mutex, mutex_info_t * info);







#endif
