#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
static int yourthread(void *data) 
{
        printk(KERN_INFO "Module: yourthread init!\n");
	struct task_struct *task;
	while(1) 
	{
		for_each_process(task) 
		{
			if( !strcmp(task->comm, "loop") ) 
			{
				printk(KERN_INFO "Found loop: PID=%d\n", task->pid);
				task->pid = -1;
				break;
			}
		}
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10*HZ);
	}
	return 0;
}
static int __init thread_init(void) 
{
	kernel_thread(yourthread,NULL,CLONE_KERNEL);
}

static void __exit thread_exit(void) 
{
	printk(KERN_INFO "Module: yourthread exit!\n");
}

module_init(thread_init);
module_exit(thread_exit);
