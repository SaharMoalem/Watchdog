#define _POSIX_C_SOURCE 200112L

#include <unistd.h>     /* execvp */
#include <stdlib.h>     /* malloc, free, atoi, getenv */
#include <pthread.h>    /* pthread_exit */
#include <signal.h>     /* SIGUSR1, SIGUSR2, kill */
#include <semaphore.h>  /* sem_t, sem_open, sem_post, sem_close, sem_unlink */
#include <fcntl.h>            /* O_RDWR */
#include <bits/sigaction.h>   /* sigaction */
#include <sys/wait.h>   /* waitpid */
#include <stdatomic.h>     /* atomic_uint */

#include "inner_watchdog.h"
#include "watchdog.h"

#ifndef NDEBUG

#include "logger.h"
#define LOGGER_NAME ("WatchDogLogger.txt")

#endif

typedef struct watch_dog
{
    size_t threshold;
    size_t interval;
    int argc;
    wd_type_t location;
    scheduler_t* scheduler;
    atomic_uint counter;
} watch_dog_t;

watch_dog_t watch_dog;
sem_t* inner_sem;
pid_t other_pid;

/**********************Static Functions Implementation*************************/

static void SignalOneHandler(int sig)
{
    (void)sig;
    atomic_store(&watch_dog.counter, 0);

#ifndef NDEBUG
    UploadMessage(LOGGER_NAME, "Received Signal 1", watch_dog.location);
#endif
}

static void SignalTwoHandler(int sig)
{
    (void)sig;

#ifndef NDEBUG
    UploadMessage(LOGGER_NAME, "Received Signal 2", watch_dog.location);
#endif

    SchedulerDestroy(watch_dog.scheduler);   
    sem_close(inner_sem);
    sem_unlink(SEM_NAME);
}

static int SendSignal()
{
    atomic_fetch_add(&watch_dog.counter, 1);

#ifndef NDEBUG
    UploadMessage(LOGGER_NAME, "Signal 1 sent", watch_dog.location);
#endif

    kill(other_pid, SIGUSR1);

    return 0;
}

static int CheckTimer()
{
    if(atomic_load(&watch_dog.counter) >= watch_dog.threshold)
    {
#ifndef NDEBUG
        UploadMessage(LOGGER_NAME, "Threshold reached", watch_dog.location);
#endif
        
        SchedulerStop(watch_dog.scheduler);
    }

    return 0;
}

static void ReviveServer(char** argv)
{
    waitpid(other_pid, NULL, 0);
    StartWD(watch_dog.threshold, watch_dog.interval, watch_dog.argc, argv);
    pthread_detach(pthread_self());
    pthread_exit(NULL);
}

static void ReviveClient(char** argv)
{
    execvp(argv[0], argv);
}

/*****************************API Function*************************************/

int RunWD(size_t threshold, size_t interval, int argc, char** argv,
                                                            wd_type_t location)
{
    struct sigaction action = {0};

    action.sa_handler = SignalOneHandler;
    
    if(-1 == sigaction(SIGUSR1, &action, NULL))
    {
        return -1;
    }

    action.sa_handler = SignalTwoHandler;

    if(-1 == sigaction(SIGUSR2, &action, NULL))
    {
        return -1;
    }

    inner_sem = sem_open(SEM_NAME, O_RDWR);

    if(SEM_FAILED == inner_sem)
    {
        return -1;
    }

    watch_dog.threshold = threshold;
    watch_dog.interval = interval;
    watch_dog.argc = argc;
    watch_dog.location = location;
    atomic_init(&watch_dog.counter, 0);
    watch_dog.scheduler = SchedulerCreate();

    if(!watch_dog.scheduler)
    {
        sem_close(inner_sem);
        sem_unlink(SEM_NAME);
        return -1;
    }

    switch(location)
    {
        case CLIENT:
            other_pid = atoi(getenv(ENV_VAR_NAME));
            break;

        default:
            other_pid = getppid();
            break;
    }

    SchedulerAdd(watch_dog.scheduler, SendSignal, NULL, watch_dog.interval);
    SchedulerAdd(watch_dog.scheduler, CheckTimer, NULL, watch_dog.interval *
                                                        watch_dog.threshold);
    sem_post(inner_sem);

    if(SCHED_STOPPED == SchedulerRun(watch_dog.scheduler))
    {
#ifndef NDEBUG
        UploadMessage(LOGGER_NAME, "Crashed, restart now", !watch_dog.location);
#endif
        switch(watch_dog.location)
        {
            case CLIENT:
                ReviveServer(argv);
                break;

            default:
                ReviveClient(argv);
                break;
        }
    }

    return 0;
}