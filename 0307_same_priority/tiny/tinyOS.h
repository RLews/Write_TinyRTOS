


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
	TaskStack_t *stack;		/* task stack */
	uint32_t delay_ticks;	/* 延时计数 */
	node_t delay_node;		/* 延时节点 */
	uint32_t state;			/* task status */
	uint32_t prio;			/* 优先级 */
	node_t link_node;		/*  */
	uint32_t slice;			/*  */
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
void TimeTaskWait(task_t *task, uint32_t ticks);
void TimeWakeUp(task_t *task);
void TaskSchedUnRdy(task_t *task);
void TaskSchedRdy(task_t *task);
void TaskDelay(uint32_t dly);
void TaskInit(task_t *task, void (*entry)(void *), void *param, uint32_t prio, TaskStack_t * stack);
void TaskSched(void);
void SetSysTickPeriod(uint32_t ms);

task_t * TaskHighestReady(void);


#endif








