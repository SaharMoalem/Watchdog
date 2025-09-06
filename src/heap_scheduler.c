#include <assert.h>     /* assert */
#include <stdlib.h>	/* malloc, free */

#include "heap_scheduler.h"	
#include "task.h"	
#include "heap_pq.h"		

typedef enum signal
{
    STOP     = 0,
    DESTROY  = 1,
    CONTINUE = 2
} signal_t;


struct scheduler
{
    heap_pq_t* queue;
    sched_status_t status;
    signal_t signal;
};

static int CompareFunc(const void* one, const void* other)
{
    assert(one);
    assert(other);
	
    return TaskGetTimeToRun((task_t*)one) - TaskGetTimeToRun((task_t*)other);
}

static int TaskUIDIsSame(const void* task, const void* uid)
{
    assert(task);
    assert(uid);
	
    return UIDIsSame(TaskGetUID(task), *(ilrd_uid_t*)uid);
}

scheduler_t* SchedulerCreate(void)
{
    scheduler_t* scheduler = (scheduler_t*)malloc(sizeof(scheduler_t));
    
    if (scheduler == NULL)
    {	
        return NULL;
    }
	
    scheduler->queue = PQCreate(CompareFunc);
    if (scheduler->queue == NULL)
    {
        free(scheduler);
      	return NULL;
    }
	
    scheduler->status = SCHED_STOPPED;
    scheduler->signal = CONTINUE;
	
    return scheduler;
}

void SchedulerDestroy(scheduler_t* scheduler)
{
    assert(scheduler);
    
    if (scheduler->status == SCHED_RUNNING)
    {
        scheduler->signal = DESTROY;
        return;
    }
    SchedulerClear(scheduler);
    PQDestroy(scheduler->queue);
    free(scheduler);
}

ilrd_uid_t SchedulerAdd(scheduler_t* scheduler,
			    int (*action_func)(void* params), void* params,
			    size_t interval_in_sec)
{
    task_t* task = NULL;
    assert(scheduler);
    assert(action_func);
	
    task = TaskCreate(action_func, params, interval_in_sec);
    if (task == NULL)
    {
      	return bad_uid;
    }
	
    if (PQEnqueue(scheduler->queue, task) != 0)
    {
      	TaskDestroy(task);
      	return bad_uid;
    }
	
    return TaskGetUID(task);
}

int SchedulerRemove(scheduler_t* scheduler, ilrd_uid_t identifier)
{
    task_t* task = NULL;
    assert(scheduler);
	
    task = PQErase(scheduler->queue, TaskUIDIsSame, &identifier); 
    if (task == NULL)
    {
      	return 1;
    }
	
    TaskDestroy(task);
	
    return 0;
}

static int TaskHandler(scheduler_t* scheduler, task_t* task)
{
    assert(scheduler);
    assert(task);
	
    if (TaskRun(task) != 0)
    {
      	TaskDestroy(task);
    }
    else
    {
      	TaskSetTimeToRun(task, time(NULL));
      	if (PQEnqueue(scheduler->queue, task) != 0)
      	{
    	    TaskDestroy(task);
	        return SCHED_ERROR;
	    }
    }
    
    return SCHED_SUCCESS;
}

sched_status_t SchedulerRun(scheduler_t* scheduler)
{
    task_t* task = NULL;
    assert(scheduler);
    
    if (scheduler->status == SCHED_RUNNING)
    {
        return SCHED_RUNNING;
    }
    
    scheduler->status = SCHED_RUNNING;
    scheduler->signal = CONTINUE;
    while (scheduler->signal == CONTINUE && !SchedulerIsEmpty(scheduler))
    {
        task = PQDequeue(scheduler->queue);
        if (TaskHandler(scheduler, task) != 0)
        {
            return SCHED_ERROR;
        }
    }
    
    switch (scheduler->signal)
    {
        case DESTROY:
            scheduler->status = SCHED_DESTROYED;
            SchedulerDestroy(scheduler);
            return SCHED_DESTROYED;
        case STOP:
            scheduler->status = SCHED_STOPPED;
            break;
        default:
            scheduler->status = SCHED_SUCCESS;
            break;
    }
    return scheduler->status;
}

void SchedulerStop(scheduler_t* scheduler)
{
    assert(scheduler);
    
    if (scheduler->signal != DESTROY)
    {
        scheduler->signal = STOP;
    }
}

size_t SchedulerSize(const scheduler_t* scheduler)
{
    assert(scheduler);
    
    return PQSize(scheduler->queue);
}

int SchedulerIsEmpty(const scheduler_t* scheduler)
{
    assert(scheduler);
    
    return PQIsEmpty(scheduler->queue);
}

void SchedulerClear(scheduler_t* scheduler)
{
    assert(scheduler);
	
    while (!SchedulerIsEmpty(scheduler))
    {
        TaskDestroy(PQDequeue(scheduler->queue));
    }
}
