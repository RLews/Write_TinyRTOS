



#include "tinyOS.h"

#if TINYOS_ENABLE_MBOX

void MboxInit(mbox_t *mbox, void **msg, uint32_t max)
{
	EventInit(&mbox->event, EVENT_TYPE_MBOX);
	mbox->msg_buf = msg;
	mbox->max = max;
	mbox->count = 0;
	mbox->read = 0;
	mbox->write = 0;
}

uint32_t MboxWait(mbox_t *mbox, void **msg, uint32_t ticks)
{
	uint32_t sta = TaskEnterCritical();
	
	if (mbox->count > 0)
	{
		mbox->count--;
		*msg = mbox->msg_buf[mbox->read++];
		if (mbox->read >= mbox->max)
		{
			mbox->read = 0;
		}
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		EventWait(&mbox->event, current_task, (void *)0, EVENT_TYPE_MBOX, ticks);
		TaskExitCritical(sta);
		TaskSched();
		*msg = current_task->event_msg;
		return current_task->wait_event_result;
	}
}

uint32_t MboxNoWaitGet(mbox_t *mbox, void **msg)
{
	uint32_t sta = TaskEnterCritical();
	
	if (mbox->count > 0)
	{
		mbox->count--;
		*msg = mbox->msg_buf[mbox->read++];
		if (mbox->read >= mbox->max)
		{
			mbox->read = 0;
		}
		TaskExitCritical(sta);
		return ERROR_NO_ERROR; 
	}
	else
	{
		TaskExitCritical(sta);
		return ERROR_RESOURCE_UNAVALIABLE;
	}
}

uint32_t MboxNotify(mbox_t *mbox, void * msg, uint32_t opt)
{
	uint32_t sta = TaskEnterCritical();
	
	if (EventWaitCount(&mbox->event) > 0)
	{
		task_t *task = EventWakeUp(&mbox->event, (void *)msg, ERROR_NO_ERROR);
		if (task->prio < current_task->prio)
		{
			TaskSched();
		}
	}
	else
	{
		if (mbox->count >= mbox->max)
		{
			TaskExitCritical(sta);
			return ERROR_RESOURCE_FULL;
		}
		
		if (opt & MBOX_SEND_FRONT)
		{
			if (mbox->read <= 0)
			{
				mbox->read = mbox->max - 1;
			}
			else
			{
				mbox->read--;
			}
			mbox->msg_buf[mbox->read] = msg;
		}
		else
		{
			mbox->msg_buf[mbox->write++] = msg;
			if (mbox->write >= mbox->max)
			{
				mbox->write = 0;
			}
		}
		mbox->count++;
	}
	
	TaskExitCritical(sta);
	return ERROR_NO_ERROR;
}

void MboxFlush(mbox_t *mbox)
{
	uint32_t sta = TaskEnterCritical();
	
	if (EventWaitCount(&mbox->event) == 0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
	
	TaskExitCritical(sta);
}

uint32_t MboxDestroy(mbox_t *mbox)
{
	uint32_t sta = TaskEnterCritical();
	uint32_t count = EventRemoveAll(&mbox->event, (void *)0, ERROR_DEL);
	
	TaskExitCritical(sta);
	
	if (count > 0)
	{
		TaskSched();
	}
	
	return count;
}

void MboxGetInfo(mbox_t *mbox, mbox_info_t *info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->count = mbox->count;
	info->max = mbox->max;
	info->task_count = EventWaitCount(&mbox->event);
	
	TaskExitCritical(sta);
}

#endif

