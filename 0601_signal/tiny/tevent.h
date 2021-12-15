

#ifndef __TEVENT_H
#define __TEVENT_H

#include "tLib.h"
#include "task.h"

typedef enum _EVENT_TYPE_T
{
	EVENT_TYPE_UNKONW,
	EVENT_TYPE_SEM,
}event_type_t;

typedef struct _EVENT_T
{
	event_type_t type;
	list_t wait_list;
}event_t;


void EventInit(event_t * event, event_type_t type);

void EventWait(event_t *event, task_t *task, void * msg, uint32_t state, uint32_t timeout);

task_t * EventWakeUp(event_t *event, void *msg, uint32_t res);

void EventRemoveTask(task_t *task, void *msg, uint32_t res);

uint32_t EventWaitCount(event_t *event);

uint32_t EventRemoveAll(event_t *event, void *msg, uint32_t res);


#endif
