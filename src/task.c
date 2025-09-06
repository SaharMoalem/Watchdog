#include <assert.h>     /* assert */
#include <time.h> 	/* time_t */
#include <stdlib.h>	/* malloc, free */
#include <unistd.h> 	/* sleep */

#include "ilrd_uid.h"
#include "task.h"

struct task
{
    ilrd_uid_t uid;
    int (*action_func)(void* params);
    void* params;
    size_t interval_in_sec;
    time_t time_to_run;
};

task_t* TaskCreate(int (*action_func)(void* params), void* params,
					    size_t interval_in_sec)
{
    task_t* task = NULL;
    assert(action_func);
	
    task = (task_t*)malloc(sizeof(task_t));
    if (task == NULL)
    {
        return NULL;
    }
	
    task->uid = UIDCreate();
    if (UIDIsSame(task->uid, bad_uid))
    {
      	free(task);
	return NULL;
    }
	
    task->action_func = action_func;
    task->params = params;
    task->interval_in_sec = interval_in_sec;
    task->time_to_run = time(NULL) + interval_in_sec;
	
    return task;
}

void TaskDestroy(task_t* task)
{
    free(task);
}

int TaskRun(task_t* task)
{
    time_t sleep_time = 0;
    assert(task);
	
    sleep_time = task->time_to_run - time(NULL);
    while (sleep_time > 0)
    {
      	sleep(sleep_time);
      	sleep_time = task->time_to_run - time(NULL);
    }
	
    return task->action_func(task->params);
}

ilrd_uid_t TaskGetUID(const task_t* task)
{
    assert(task);
    
    return task->uid;
}

size_t TaskGetInterval(const task_t* task)
{
    assert(task);
    
    return task->interval_in_sec;
}

time_t TaskGetTimeToRun(const task_t* task)
{
    assert(task);
	
    return task->time_to_run;
}

void TaskSetTimeToRun(task_t* task)
{
    assert(task);
	
    task->time_to_run = time(NULL) + task->interval_in_sec;
}

int TaskIsEqual(const task_t* task1, const task_t* task2)
{
    if (task1 == NULL || task2 == NULL)
    {
        return task1 == task2;
    }
    
    return UIDIsSame(task1->uid, task2->uid);
}
