



#ifndef __HOOKS_H
#define __HOOKS_H

#include "tevent.h"

void HooksCpuIdle(void);

void HooksSystick(void);

void HooksTaskSwitch(task_t *frome, task_t *to);

void HooksTaskInit(task_t *task);




#endif

