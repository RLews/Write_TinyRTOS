

#ifndef __SEM_T
#define __SEM_H

#include "tevent.h"



typedef struct _SEM_T
{
	event_t event;
	uint32_t count;
	uint32_t max;
}sem_t;

typedef struct _SEM_INFO_T
{
	uint32_t count;
	uint32_t max;
	uint32_t task_cnt;
}sem_info_t;

void SemInit(sem_t *sem, uint32_t start_cnt, uint32_t max);

uint32_t SemWait(sem_t *sem, uint32_t wait_ticks);

uint32_t SemNoWait(sem_t *sem);

void SemNotify(sem_t *sem);

void SemGetInfo(sem_t *sem, sem_info_t *info);

uint32_t SemDestory(sem_t *sem);



#endif
