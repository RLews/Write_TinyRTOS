

#ifndef __TASK_H
#define __TASK_H



#define TINYOS_TASK_STATE_READY			0
#define TINYOS_TASK_STATE_DESTORYED		(1 << 1)
#define TINYOS_TASK_STATE_DELAYED		(1 << 2)
#define TINYOS_TASK_STATE_SUSPEND		(1 << 3)

#define TINYOS_TASK_WAIT_MASK			(0xFF << 16)

struct _EVENT_T;

typedef uint32_t TaskStack_t;

typedef struct _TASK_T
{
	TaskStack_t *stack;		/* task stack */
	uint32_t delay_ticks;	/* 延时计数 */
	node_t delay_node;		/* 延时节点 */
	uint32_t state;			/* task status */
	uint32_t prio;			/* 优先级 */
	node_t link_node;		/*  */
	uint32_t slice;			/* 剩余时间片 */
	uint32_t suspend_cnt;	/* suspend counter */
	
	void (*clean) (void * param);
	void *clean_param;
	uint8_t request_delflag;
	
	struct _EVENT_T *wait_event;
	void * event_msg;
	uint32_t wait_event_result;
	
	uint32_t event_flags;
	uint32_t wait_flags_type;
}task_t;

typedef struct _TASK_INFOR_T
{
	uint32_t delay_ticks;
	uint32_t prio;
	uint32_t state;
	uint32_t slice;
	uint32_t suspend_cnt;
}task_infor_t;



void TaskSchedUnRdy(task_t *task);
void TaskSchedRdy(task_t *task);
void TaskInit(task_t *task, void (*entry)(void *), void *param, uint32_t prio, TaskStack_t * stack);
void TaskSched(void);
void TaskWakeUp(task_t *task);
void TaskSuspend(task_t *task);
task_t * TaskHighestReady(void);
void TimeTaskRemove(task_t *task);
void TaskSchedRemove(task_t * task);
void TaskSetCleanCallBack(task_t *task, void (*clean)(void *param), void * param);
void TaskForceDelete(task_t * task);
void TaskRequestDelete(task_t *task);
uint8_t TaskIsRequestDeleted(void);
void TaskDeleteSelf(void);
void TaskGetInfo(task_t *task, task_infor_t *info);
void TaskSchedInit(void);
void TaskSchedEnable(void);
void TaskSchedDisable(void);










#endif
