

#include "tinyOS.h"
#include "stm32f10x.h"
#include "core_cm3.h"

#if TINYOS_ENABLE_CPUUSAGE
static uint32_t EnableCpuUsageState = 0;
static float cpuUsage = 0.0;
uint32_t idlecount = 0;
static uint32_t idle_maxcount = 0;
static uint32_t tickcount = 0;

static void CheckCpuUsage(void);
#endif

void SetSysTickPeriod(uint32_t ms)
{
	u32 reload;
	//u8 fac_us;
	//u16 fac_ms;
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//选择外部时钟  HCLK/8
	//fac_us=SystemCoreClock/8000000;	//为系统时钟的1/8  
	 
	reload=SystemCoreClock/8000000;		//每秒钟的计数次数 单位为K	   
	reload*=1000000/TINY_OS_TICKS_PER_SEC;//根据OS_TICKS_PER_SEC设定溢出时间
							//reload为24位寄存器,最大值:16777216,在72M下,约合1.86s左右	
	//fac_ms=1000/TINY_OS_TICKS_PER_SEC;//代表ucos可以延时的最少单位	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//开启SYSTICK中断
	SysTick->LOAD=reload; 	//每1/OS_TICKS_PER_SEC秒中断一次	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//开启SYSTICK   
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级
}

void SysTick_Handler(void)
{
	TaskSystemTickHandler();
}

void TaskSystemTickHandler(void)
{
	node_t *node;
	uint32_t status = TaskEnterCritical();
	
	for (node = task_delayqueue.head.next; node != &(task_delayqueue.head); node = node->next)
	{
		task_t *task = NODE_PARENT(node, task_t, delay_node);
		if (--task->delay_ticks == 0)
		{
			if (task->wait_event)
			{
				EventRemoveTask(task, (void *)0, ERROR_TIMEOUT);
			}
			
			TimeWakeUp(task);
			TaskSchedRdy(task);
		}
	}
	
	if (--current_task->slice == 0)
	{
		if (ListCount(&task_table[current_task->prio]) > 0)
		{
			ListRemoveFirst(&task_table[current_task->prio]);
			ListAddLast(&task_table[current_task->prio], &(current_task->link_node));
			current_task->slice = TINY_OS_SLICE_MAX;
		}
	}
#if TINYOS_ENABLE_CPUUSAGE
	tickcount++;
	CheckCpuUsage();
#endif
	TaskExitCritical(status);
#if TINYOS_ENABLE_STIMER
	StimerModuleTickNotify();
#endif
	
#if TINYOS_ENABLE_HOOKS
	HooksSystick();
#endif
	TaskSched();
	
}



#if TINYOS_ENABLE_CPUUSAGE
void CpuCountplus(void)
{
	idlecount++;
}

void TimeTickCountInit(void)
{
	tickcount = 0;
}

void InitCpuUsageState(void)
{
	EnableCpuUsageState = 0;
	idlecount = 0;
	idle_maxcount = 0;
	cpuUsage = 0.0f;
}

static void CheckCpuUsage(void)
{
	if (EnableCpuUsageState == 0)
	{
		EnableCpuUsageState = 1;
		tickcount = 0;
		return;
	}
	
	if (tickcount == SYSTICK_PRE_SEC)
	{
		idle_maxcount = idlecount;
		idlecount = 0;
		
		TaskSchedEnable();
	}
	else if (tickcount % SYSTICK_PRE_SEC == 0)
	{
		cpuUsage = 100 - ((idlecount * 100.0) / idle_maxcount);
		idlecount = 0;
	}
	
}

void CpuUsageSyncWithSystick(void)
{
	while (EnableCpuUsageState == 0)
	{
		;
	}
}

float CpuUsageGet (void)
{
	float usage = 0.0;
	uint32_t status = TaskEnterCritical();
	usage = cpuUsage;
	TaskExitCritical(status);
	
	return usage;
}
#endif


