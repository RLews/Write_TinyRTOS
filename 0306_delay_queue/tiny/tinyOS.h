


#ifndef __TINY_OS_H
#define __TINY_OS_H

#include <stdint.h>
#include "tLib.h"
#include "tconfig.h"

#define TINYOS_TASK_STATE_READY			0
#define TINYOS_TASK_STATE_DELAYED		(1 << 1)

typedef uint32_t TaskStack_t;

typedef struct _TASK_T
{
	TaskStack_t *stack;
	uint32_t delay_ticks;
	node_t delay_node;
	uint32_t state;
	uint32_t prio;
}task_t;


extern task_t *crrent_task;
extern task_t *next_task;

void TaskRunFirst(void);
void TaskSwitch(void);
uint32_t TaskEnterCritical(void);
void TaskExitCritical(uint32_t sta);
void TaskSchedInit(void);
void TaskSchedEnable(void);
void TaskSchedDisable(void);

#endif
