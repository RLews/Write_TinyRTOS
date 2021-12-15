


#ifndef __TINY_OS_H
#define __TINY_OS_H

#include <stdint.h>
#include "tLib.h"
#include "tconfig.h"
#include "task.h"
#include "tevent.h"
#include "sem.h"
#include "mail.h"
#include "mem.h"
#include "FlagGroup.h"
#include "mutex.h"

typedef enum _ERROT_T
{
	ERROR_NO_ERROR = 0,
	ERROR_TIMEOUT,
	ERROR_RESOURCE_UNAVALIABLE,
	ERROR_DEL,
	ERROR_RESOURCE_FULL,
	ERROR_OWNER,
}error_t;



extern task_t *current_task;
extern task_t *next_task;
extern list_t task_table[TINY_OS_PRO_COUNT];
extern list_t task_delayqueue;

void TaskRunFirst(void);
void TaskSwitch(void);
uint32_t TaskEnterCritical(void);
void TaskExitCritical(uint32_t sta);

void TimeTaskWait(task_t *task, uint32_t ticks);
void TimeWakeUp(task_t *task);

void TaskDelay(uint32_t dly);
void SetSysTickPeriod(uint32_t ms);
void TaskSystemTickHandler(void);
void TaskDelayedInit(void);



void AppInit(void);
void delay(uint32_t cnt);

#endif








