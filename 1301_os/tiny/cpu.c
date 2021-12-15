

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
	
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	//fac_us=SystemCoreClock/8000000;	//Ϊϵͳʱ�ӵ�1/8  
	 
	reload=SystemCoreClock/8000000;		//ÿ���ӵļ������� ��λΪK	   
	reload*=1000000/TINY_OS_TICKS_PER_SEC;//����OS_TICKS_PER_SEC�趨���ʱ��
							//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����	
	//fac_ms=1000/TINY_OS_TICKS_PER_SEC;//����ucos������ʱ�����ٵ�λ	   
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD=reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;   	//����SYSTICK   
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
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


