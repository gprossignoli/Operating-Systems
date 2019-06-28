#include "sched.h"
//void sched_init_ <name >( void );
//void sched_destroy_ <name >( void ) ;
//static void task_new_ <name >( task_t * t );
//static void task_free_ <name >( task_t * t );


static void enqueue_task_fcfs( task_t * t, runqueue_t * rq ,
int preempted ){

	if(t->on_rq || is_idle_task(t))
		return;

	insert_slist(&rq->tasks,t);
}

static task_t * pick_next_task_fcfs( runqueue_t * rq ){

	task_t* nextTask  = head_slist(&rq->tasks);
	if(nextTask)
		remove_slist(&rq->tasks,nextTask);

	return nextTask;
}
//static void task_tick_ <name >( runqueue_t * rq );

static task_t * steal_task_fcfs( runqueue_t * rq ){

	task_t* lastTask = tail_slist(&rq->tasks);

	if(lastTask)
		remove_slist(&rq->tasks,lastTask);

	return lastTask;

}

sched_class_t fcfs_sched= {
	.pick_next_task=pick_next_task_fcfs,
	.enqueue_task=enqueue_task_fcfs,
	.steal_task=steal_task_fcfs
};
