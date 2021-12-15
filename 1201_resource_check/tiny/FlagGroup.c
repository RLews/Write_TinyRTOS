



#include "tinyOS.h"

static uint32_t FlagGroupCheckAndConsume(flag_group_t *group, uint32_t type, uint32_t *flag);


void FlagGroupInit(flag_group_t * group, uint32_t flags)
{
	EventInit(&group->event, EVENT_TYPE_FLAG_GROUP);
	group->flag = flags;
}

static uint32_t FlagGroupCheckAndConsume(flag_group_t *group, uint32_t type, uint32_t *flag)
{
	uint32_t srcflag = *flag;
	uint32_t isSet = type & TFLAGGROUP_SET;
	uint32_t isAll = type & TFLAGGROUP_ALL;
	uint32_t isConsume = type & TFLAGGROUP_CONSUME;
	
	uint32_t calflag = isSet ? (group->flag & srcflag) : (~group->flag & srcflag);
	
	if ( ((isAll != 0) && (calflag == srcflag)) || ((isAll == 0) && (calflag != 0)) )
	{
		if (isConsume)
		{
			if (isSet)
			{
				group->flag &= ~srcflag;
			}
			else
			{
				group->flag |= srcflag;
			}
		}
		
		*flag = calflag;
		return ERROR_NO_ERROR;
	}
	
	*flag = calflag;
	return ERROR_RESOURCE_UNAVALIABLE;	
}

uint32_t FlagGroupWait(flag_group_t *group, uint32_t type, uint32_t reqflag, uint32_t * resflag, uint32_t ticks)
{
	uint32_t flags = reqflag;
	uint32_t sta = TaskEnterCritical();
	uint32_t result = FlagGroupCheckAndConsume(group, type, &flags);
	
	if (result != ERROR_NO_ERROR)
	{
		current_task->wait_flags_type = type;
		current_task->event_flags = reqflag;
		EventWait(&group->event, current_task, (void *)0, EVENT_TYPE_FLAG_GROUP, ticks);
		TaskExitCritical(sta);
		TaskSched();
		*resflag = current_task->event_flags;
		result = current_task->wait_event_result;
	}
	else
	{
		*resflag = flags;
		TaskExitCritical(sta);
	}
	
	return result;
}

uint32_t FlagGroupNoWaitGet(flag_group_t *group, uint32_t type, uint32_t reqflag, uint32_t * resflag)
{
	uint32_t flags = reqflag;
	uint32_t sta = TaskEnterCritical();
	uint32_t result = FlagGroupCheckAndConsume(group, type, &flags);

	TaskExitCritical(sta);
	
	*resflag = flags;
	return ERROR_NO_ERROR;
}

void FlagGroupNotify(flag_group_t *group, uint8_t isset, uint32_t flag)
{
	list_t *waitlist;
	node_t *node;
	node_t *nextnode;
	uint32_t result;
	uint8_t sched = 0;
	uint32_t sta = TaskEnterCritical();
	
	if (isset)
	{
		group->flag |= flag;
	}
	else
	{
		group->flag &= ~flag;
	}
	
	waitlist = &group->event.wait_list;
	for (node = waitlist->head.next; node != &(waitlist->head); node = nextnode)
	{
		task_t *task = NODE_PARENT(node, task_t, link_node);
		uint32_t flags = task->event_flags;
		nextnode = node->next;
		
		result = FlagGroupCheckAndConsume(group, task->wait_flags_type, &flags);
		if (result == ERROR_NO_ERROR)
		{
			task->event_flags = flags;
			EventWakeUpTask(&group->event, task, (void *)0, ERROR_NO_ERROR);
			sched = 1;
		}
	}
	
	if (sched == 1)
	{
		TaskSched();
	}
	
	TaskExitCritical(sta);
}


void FlagGroupGetInfo(flag_group_t *group, flag_group_info_t *info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->flags = group->flag;
	info->taskcnt = EventWaitCount(&group->event);
	
	TaskExitCritical(sta);
}

uint32_t FlagGroupDestroy(flag_group_t *group)
{
	uint32_t sta = TaskEnterCritical();
	
	uint32_t count = EventRemoveAll(&group->event, (void *)0, ERROR_DEL);
	
	TaskExitCritical(sta);
	
	if (count > 0)
	{
		TaskSched();
	}
	
	return count;
}

















