



#include "tinyOS.h"

void MemBlockInit(mem_block_t * mem, uint8_t *memstart, uint32_t blocksize, uint32_t blockcnt)
{
	uint8_t * memblockst = (uint8_t *)memstart;
	uint8_t * memblockend = memblockst + blocksize * blockcnt;
	
	if (blocksize < sizeof(node_t))
	{
		return;
	}
	
	EventInit(&mem->event, EVENT_TYPE_MEM);
	mem->mem_start = memstart;
	mem->blocksize = blocksize;
	mem->maxcount = blockcnt;
	
	ListInit(&mem->blocklist);
	while (memblockst < memblockend)
	{
		NodeInit((node_t *)memblockst);
		ListAddLast(&mem->blocklist, (node_t *)memblockst);
		memblockst += blocksize;
	}
}

uint32_t MemBlockWait(mem_block_t *memblock, uint8_t **mem, uint32_t waitticks)
{
	uint32_t sta = TaskEnterCritical();
	
	if (ListCount(&memblock->blocklist) > 0)
	{
		*mem = (uint8_t *)ListRemoveFirst(&memblock->blocklist);
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		EventWait(&memblock->event, current_task, (void*)0, EVENT_TYPE_MEM, waitticks);
		TaskExitCritical(sta);
		TaskSched();
		*mem = current_task->event_msg;
		return current_task->wait_event_result;
	}
	
}

uint32_t MemBlockNoWaitGet(mem_block_t *memblock, uint8_t **mem)
{
	uint32_t sta = TaskEnterCritical();
	
	if (ListCount(&memblock->blocklist) > 0)
	{
		*mem = (uint8_t *)ListRemoveFirst(&memblock->blocklist);
		TaskExitCritical(sta);
		return ERROR_NO_ERROR;
	}
	else
	{
		
		TaskExitCritical(sta);
		return ERROR_RESOURCE_UNAVALIABLE;
	}
	
}

void MemBlockNotify(mem_block_t *memblock, uint8_t * mem)
{
	uint32_t sta = TaskEnterCritical();
	
	if (EventWaitCount(&memblock->event) > 0)
	{
		task_t *task = EventWakeUp(&memblock->event, (void *)mem, ERROR_NO_ERROR);
		if (task->prio < current_task->prio)
		{
			TaskSched();
		}
	}
	else
	{
		ListAddLast(&memblock->blocklist, (node_t *)mem);
	}
	
	TaskExitCritical(sta);
}

void MemBlockGetInfo(mem_block_t *memblock, mem_block_info_t *info)
{
	uint32_t sta = TaskEnterCritical();
	
	info->blocksize = memblock->blocksize;
	info->count = ListCount(&memblock->blocklist);
	info->max = memblock->maxcount;
	info->taskcount = EventWaitCount(&memblock->event);
	
	TaskExitCritical(sta);
}

uint32_t MemBlockDestroy(mem_block_t *memblock)
{
	uint32_t sta = TaskEnterCritical();
	
	uint32_t count = EventRemoveAll(&memblock->event, (void *)0, ERROR_DEL);
	
	TaskExitCritical(sta);
	
	if (count > 0)
	{
		TaskSched();
	}
	return count;
}







