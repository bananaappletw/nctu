#include <kernel/task.h>
#include <inc/lock.h>
static index = 0;
void sched_yield(void)
{

	extern Task tasks[];
	extern Task *cur_task;
	int i;
	int next_i = 0;
	/* Lab4 TODO: Implement a simple round-robin scheduling there 
	*  Hint: Choose a runnable task from tasks[] and use env_pop_tf() do the context-switch
	*/
	for( i=0 ; i < NR_TASKS ; i++ )
	{
		index = (index +1) % NR_TASKS;
		if( tasks[index].state == TASK_RUNNABLE )
		{	
			lock();
			if(cur_task!=NULL && cur_task->state==TASK_RUNNING)
				cur_task->state = TASK_RUNNABLE;
			cur_task=&tasks[index];
			cur_task->state = TASK_RUNNING;
			cur_task->remind_ticks= TIME_QUANT;
			env_pop_tf(&cur_task->tf);
		}
	}

}
