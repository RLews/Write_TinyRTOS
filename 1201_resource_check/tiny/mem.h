

#ifndef __MEM_H
#define __MEM_H

#include "tevent.h"


typedef struct _MEM_BLOCK_T
{
	event_t event;
	void *mem_start;
	uint32_t blocksize;
	uint32_t maxcount;
	list_t blocklist;
}mem_block_t;

typedef struct _MEM_BLOCK_INFO_T
{
	uint32_t count;
	uint32_t max;
	uint32_t blocksize;
	uint32_t taskcount;
}mem_block_info_t;


void MemBlockInit(mem_block_t * mem, uint8_t *memstart, uint32_t blocksize, uint32_t blockcnt);

uint32_t MemBlockWait(mem_block_t *memblock, uint8_t **mem, uint32_t waitticks);

uint32_t MemBlockNoWaitGet(mem_block_t *memblock, uint8_t **mem);

void MemBlockNotify(mem_block_t *memblock, uint8_t * mem);

void MemBlockGetInfo(mem_block_t *memblock, mem_block_info_t *info);

uint32_t MemBlockDestroy(mem_block_t *memblock);




#endif
