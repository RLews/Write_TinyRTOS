



#ifndef __STIMER_H
#define __STIMER_H

#include "tevent.h"

typedef enum _STIMER_STATE_T
{
	STIMER_CREATED,
	STIMER_STARTED,
	STIMER_RUNNING,
	STIMER_STOPPED,
	STIMER_DESTROYED
}stimer_state_t;

typedef struct _STIMER_T
{
	node_t link_node;
	uint32_t start_delay_ticks;
	uint32_t duration_ticks;
	uint32_t delay_ticks;
	void (*timerfunc) (void * arg);
	void *arg;
	uint32_t config;
	stimer_state_t state;
}stimer_t;

typedef struct _STIMER_INFO_T
{
	uint32_t start_delay_ticks;
	uint32_t duration_ticks;
	void (*timerfunc) (void * arg);
	void *arg;
	uint32_t config;
	stimer_state_t state;
}stimer_info_t;

#define STIMER_CONFIG_TYPE_HARD			(1 << 0)
#define STIMER_CONFIG_TYPE_SOFT			(0 << 0)


void StimerInit(stimer_t *timer, uint32_t delayticks, uint32_t durticks, void (*func)(void *arg), void * arg, uint32_t config);

void StimerModuleInit(void);

void StimerModuleTickNotify(void);

void StimerStart(stimer_t *timer);

void StimerStop(stimer_t *timer);

void StimerDestroy(stimer_t *timer);

void StimerGetInfo(stimer_t *timer, stimer_info_t *info);

void StimerInitTask(void);






#endif

