


#ifndef __TINY_OS_H
#define __TINY_OS_H

#include <stdint.h>

typedef uint32_t TaskStack_t;

typedef struct _TASK_T
{
	TaskStack_t *stack;
}task_t;


extern task_t *crrent_task;
extern task_t *next_task;

void TaskRunFirst(void);
void TaskSwitch(void);


#endif
