



#include "tinyOS.h"

#if TINYOS_ENABLE_STIMER


static list_t s_timer_hardlist;
static list_t s_timer_softlist;
static sem_t s_timer_protectsem;
static sem_t s_timer_ticksem;

static task_t timertask;
static TaskStack_t timertask_stack[TINY_OS_STIMERTASK_STACK_SIZE];


void StimerInit(stimer_t *timer, uint32_t delayticks, uint32_t durticks, void (*func)(void *arg), void * arg, uint32_t config)
{
	NodeInit(&timer->link_node);
	timer->start_delay_ticks = delayticks;
	timer->duration_ticks = durticks;
	timer->timerfunc = func;
	timer->arg = arg;
	timer->config = config;
	
	if (delayticks == 0)
	{
		timer->delay_ticks = durticks;
	}
	else
	{
		timer->delay_ticks = timer->start_delay_ticks;
	}
	
	timer->state = STIMER_CREATED;
}

void StimerStart(stimer_t *timer)
{
	switch (timer->state)
	{
		case STIMER_CREATED:
		case STIMER_STOPPED:
			timer->delay_ticks = timer->start_delay_ticks ? timer->start_delay_ticks : timer->duration_ticks;
			timer->state = STIMER_STARTED;
		
			if (timer->config & STIMER_CONFIG_TYPE_HARD)
			{
				uint32_t sta = TaskEnterCritical();
				
				ListAddFirst(&s_timer_hardlist, &timer->link_node);
				
				TaskExitCritical(sta);
			}
			else
			{
				SemWait(&s_timer_protectsem, 0);
				ListAddLast(&s_timer_softlist, &timer->link_node);
				SemNotify(&s_timer_protectsem);
			}
			break;
		default:
			break;
	}
}

void StimerStop(stimer_t *timer)
{
	switch (timer->state)
	{
	
		case STIMER_STARTED:
		case STIMER_RUNNING:
			if (timer->config & STIMER_CONFIG_TYPE_HARD)
			{
				uint32_t sta = TaskEnterCritical();
				ListRemove(&s_timer_hardlist, &timer->link_node);
				TaskExitCritical(sta);
			}
			else
			{
				SemWait(&s_timer_protectsem, 0);
				ListRemove(&s_timer_softlist, &timer->link_node);
				SemNotify(&s_timer_protectsem);
			}
					
			break;
		default:
			break;
	}
}

static void StimerCallFuncList(list_t * timerlist)
{
	node_t *node;
	for (node = timerlist->head.next; node != &(timerlist->head); node = node->next)
	{
		stimer_t *timer = NODE_PARENT(node, stimer_t, link_node);
		if ( (timer->delay_ticks == 0) || (--timer->delay_ticks == 0) )
		{
			timer->state = STIMER_RUNNING;
			timer->timerfunc(timer->arg);
			timer->state = STIMER_STARTED;
			
			if (timer->duration_ticks > 0)
			{
				timer->delay_ticks = timer->duration_ticks;
			}
			else
			{
				ListRemove(timerlist, &timer->link_node);
				timer->state = STIMER_STOPPED;
			}
		}
	}
}

static void StimerSoftTask(void *param)
{
	while (1)
	{
		SemWait(&s_timer_ticksem, 0);
		SemWait(&s_timer_protectsem, 0);
		
		StimerCallFuncList(&s_timer_softlist);
		
		SemNotify(&s_timer_protectsem);
	}
}

void StimerModuleTickNotify(void)
{
	uint32_t sta = TaskEnterCritical();
	
	StimerCallFuncList(&s_timer_hardlist);
	
	TaskExitCritical(sta);
	
	SemNotify(&s_timer_ticksem);
}

void StimerModuleInit(void)
{
	ListInit(&s_timer_hardlist);
	ListInit(&s_timer_softlist);
	SemInit(&s_timer_protectsem, 1, 1);
	SemInit(&s_timer_ticksem, 0, 0);
}

void StimerInitTask(void)
{
#if TINY_OS_STIMER_TASK_PRIO >= (TINY_OS_PRO_COUNT - 1)
	#error "the proprity of timer task must be greater then (TINY_OS_PRO_COUNT - 1)"
#endif
	TaskInit(&timertask, StimerSoftTask, (void *)0, TINY_OS_STIMER_TASK_PRIO, timertask_stack, sizeof(timertask_stack));
}

void StimerDestroy(stimer_t *timer)
{
	StimerStop(timer);
	timer->state = STIMER_DESTROYED;
}

void StimerGetInfo(stimer_t *timer, stimer_info_t *info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->arg = timer->arg;
	info->config = timer->config;
	info->duration_ticks = timer->duration_ticks;
	info->start_delay_ticks = timer->start_delay_ticks;
	info->state = timer->state;
	info->timerfunc = timer->timerfunc;
	
	SemNotify(&s_timer_ticksem);
}






#endif



