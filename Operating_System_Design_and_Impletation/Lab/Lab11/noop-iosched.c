/*
 * elevator noop
 */
#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/init.h>

struct noop_data {
	struct list_head queue;
};

static void noop_merged_requests(struct request_queue *q, struct request *rq,
				 struct request *next)
{
	list_del_init(&next->queuelist);
}

static int noop_dispatch(struct request_queue *q, int force)
{
	struct noop_data *nd = q->elevator->elevator_data;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		//LAB11
		printk("dispatch %llu\n", blk_rq_pos(rq));
		list_del_init(&rq->queuelist);		
		elv_dispatch_sort(q, rq);
		return 1;
	}
	return 0;
}

static void noop_add_request(struct request_queue *q, struct request *rq)
{
	struct noop_data *nd = q->elevator->elevator_data;
	unsigned long long new_to_last_distance,cur_to_last_distance;
	unsigned long long last_pos=q->end_sector;
	unsigned long long cur_pos;
	unsigned long long new_pos=blk_rq_pos(rq);
	struct list_head *req; 
    struct request *temp;
	int flag=0;
	//LAB11
	printk("add %llu\n", blk_rq_pos(rq));
	if(list_empty(&nd->queue))
	{
		list_add_tail(&rq->queuelist, &nd->queue);
	}
	else
	{
        list_for_each(req, &nd->queue) 
		{
			temp=list_entry(req, struct request, queuelist);
			cur_pos=blk_rq_pos(temp);
			if(cur_pos>=last_pos)
				cur_to_last_distance=cur_pos-last_pos;
			else
				cur_to_last_distance=last_pos-cur_pos;				
			if(new_pos>=last_pos)
				new_to_last_distance=new_pos-last_pos;
			else				
				new_to_last_distance=last_pos-new_pos;
			if(new_to_last_distance<=cur_to_last_distance)
			{
				list_add_tail(&rq->queuelist,req);
				flag=1;
				break;
			}
			last_pos=cur_pos;
		}
		if(flag==0)
		{
			list_add_tail(&rq->queuelist, req);		
		}
	}
}

static int noop_queue_empty(struct request_queue *q)
{
	struct noop_data *nd = q->elevator->elevator_data;

	return list_empty(&nd->queue);
}

static struct request *
noop_former_request(struct request_queue *q, struct request *rq)
{
	struct noop_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *
noop_latter_request(struct request_queue *q, struct request *rq)
{
	struct noop_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue)
		return NULL;
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static void *noop_init_queue(struct request_queue *q)
{
	struct noop_data *nd;

	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd)
		return NULL;
	INIT_LIST_HEAD(&nd->queue);
	return nd;
}

static void noop_exit_queue(struct elevator_queue *e)
{
	struct noop_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_noop = {
	.ops = {
		.elevator_merge_req_fn		= noop_merged_requests,
		.elevator_dispatch_fn		= noop_dispatch,
		.elevator_add_req_fn		= noop_add_request,
		.elevator_queue_empty_fn	= noop_queue_empty,
		.elevator_former_req_fn		= noop_former_request,
		.elevator_latter_req_fn		= noop_latter_request,
		.elevator_init_fn		= noop_init_queue,
		.elevator_exit_fn		= noop_exit_queue,
	},
	.elevator_name = "noop",
	.elevator_owner = THIS_MODULE,
};

static int __init noop_init(void)
{
	elv_register(&elevator_noop);

	return 0;
}

static void __exit noop_exit(void)
{
	elv_unregister(&elevator_noop);
}

module_init(noop_init);
module_exit(noop_exit);


MODULE_AUTHOR("Jens Axboe");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("No-op IO scheduler");
