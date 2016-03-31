/* Reference: http://www.osdever.net/bkerndev/Docs/pit.htm */
#include <kernel/trap.h>
#include <kernel/picirq.h>
#include <kernel/task.h>
#include <inc/mmu.h>
#include <inc/x86.h>
#include <inc/lock.h>

#define TIME_HZ 100

static unsigned long jiffies = 0;

void set_timer(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

/* It is timer interrupt handler */
void timer_handler(struct Trapframe *tf)
{
	int i;

	jiffies++;

	extern Task tasks[];
	
	extern Task *cur_task;
	if (cur_task != NULL)
	{
		/* Lab4 TODO: Check if tasks need wakeup.  */
		for( i=0 ; i < NR_TASKS ; i++ )
		{
			if(tasks[i].state == TASK_SLEEP)
			{
				tasks[i].remind_ticks--;
				if(tasks[i].remind_ticks==0)
					tasks[i].state=TASK_RUNNABLE;
			}
		}
		/* Lab4 TODO: Check cur_task->remind_ticks, if remind_ticks <= 0 then yield the task (call sched_yield() in sched.c)*/
		cur_task->remind_ticks--;
		if(cur_task->remind_ticks<=0)
		{
			if((tf->tf_cs & 3) == 3)
				sched_yield();
			else
				env_pop_tf(tf);
		}
	}
}

unsigned long get_tick()
{
	return jiffies;
}
void timer_init()
{
	set_timer(TIME_HZ);

	/* Enable interrupt */
	irq_setmask_8259A(irq_mask_8259A & ~(1<<IRQ_TIMER));

	/* Lab3 TODO: Register trap handler */
	extern void timer_entry();
	
	register_handler(IRQ_OFFSET+IRQ_TIMER, timer_handler,timer_entry, 0, 0);
	//SETGATE(idt[IRQ_OFFSET+IRQ_TIMER], 0, GD_KT, timer_entry, 0)

}

