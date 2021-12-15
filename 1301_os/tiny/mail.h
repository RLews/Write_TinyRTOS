

#ifndef __MAIL_H
#define __MAIL_H

#include "tevent.h"

#define MBOX_SEND_NORMAL	0x00
#define MBOX_SEND_FRONT		0x01


typedef struct _MBOX_T
{
	event_t event;
	uint32_t count;
	uint32_t read;
	uint32_t write;
	uint32_t max;
	void ** msg_buf;
}mbox_t;

typedef struct _MBOX_INFO_T
{
	uint32_t count;
	uint32_t max;
	uint32_t task_count;
}mbox_info_t;


void MboxInit(mbox_t *mbox, void **msg, uint32_t max);

uint32_t MboxWait(mbox_t *mbox, void **msg, uint32_t ticks);

uint32_t MboxNoWaitGet(mbox_t *mbox, void **msg);

uint32_t MboxNotify(mbox_t *mbox, void * msg, uint32_t opt);

void MboxFlush(mbox_t *mbox);

uint32_t MboxDestroy(mbox_t *mbox);

void MboxGetInfo(mbox_t *mbox, mbox_info_t *info);





#endif

