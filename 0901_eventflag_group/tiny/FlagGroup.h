


#ifndef __FLAG_GROUP_H
#define __FLAG_GROUP_H

#include "tevent.h"

#define TFLAGGROUP_CLEAR				(0x00 << 0)
#define TFLAGGROUP_SET					(0x01 << 0)
#define TFLAGGROUP_ANY					(0x00 << 1)
#define TFLAGGROUP_ALL					(0x01 << 1)

#define TFLAGGROUP_SET_ALL				(TFLAGGROUP_SET | TFLAGGROUP_ALL)
#define TFLAGGROUP_SET_ANY				(TFLAGGROUP_SET | TFLAGGROUP_ANY)
#define TFLAGGROUP_CLEAR_ALL			(TFLAGGROUP_CLEAR | TFLAGGROUP_ALL)
#define TFLAGGROUP_CLEAR_ANY			(TFLAGGROUP_CLEAR | TFLAGGROUP_ANY)

#define TFLAGGROUP_CONSUME				(1 << 7)

typedef struct _FLAG_GROUP_T
{
	event_t event;
	uint32_t flag;
}flag_group_t;

typedef struct _FLAG_GROUP_INFO_T
{
	uint32_t flags;
	uint32_t taskcnt;
}flag_group_info_t;


void FlagGroupInit(flag_group_t * group, uint32_t flags);

uint32_t FlagGroupWait(flag_group_t *group, uint32_t type, uint32_t reqflag, uint32_t * resflag, uint32_t ticks);

uint32_t FlagGroupNoWaitGet(flag_group_t *group, uint32_t type, uint32_t reqflag, uint32_t * resflag);

void FlagGroupNotify(flag_group_t *group, uint8_t isset, uint32_t flag);

void FlagGroupGetInfo(flag_group_t *group, flag_group_info_t *info);

uint32_t FlagGroupDestroy(flag_group_t *group);



#endif
