#define _POSIX_C_SOURCE 200112L

#include <unistd.h>     /* fork, execvp */
#include <stdlib.h>     /* malloc, free */
#include <pthread.h>    /* pthread_t, pthread_create, pthread_join */
#include <signal.h>     /* SIGUSR2, kill */
#include <semaphore.h>  /* sem_t, sem_open, sem_wait, sem_close, sem_unlink */
#include <stdio.h>      /* fprintf, sprintf */
#include <fcntl.h>      /* O_CREAT */
#include <assert.h>     /* assert */
#include <sys/wait.h>   /* waitpid */

#include "watchdog.h"
#include "inner_watchdog.h"

#define TOTAL_INPUT_TO_EXCPECT (5)

size_t g_threshold;
size_t g_interval;
int g_argc;
pid_t pid;
pthread_t thread;

/**********************Static Functions Implementation*************************/

static void* ThreadStart(void* args)
{
    char** arguments = (char**)args;

    if(-1 == RunWD(g_threshold, g_interval, g_argc, arguments, CLIENT))
    {
        fprintf(stderr, "Thread creation failed\n");
        return NULL;
    }

    return NULL;
}

static void CleanResources(sem_t* sem, char** args)
{
    free(args);
    sem_close(sem);
    sem_unlink(SEM_NAME);
}

static char** CreateExecArgsInput(char* threshold, char* interval, char* argc,
                                                                    char** argv)
{
    char** output = malloc((g_argc + TOTAL_INPUT_TO_EXCPECT) * sizeof(char*));
    int i = 0;

    if(!output)
    {
        return NULL;
    }

    sprintf(threshold, "%lu" ,g_threshold);
    sprintf(interval, "%lu" ,g_interval);
    sprintf(argc, "%d" ,g_argc);

    output[0] = EXEC_FILE_RUN;
    output[1] = threshold;
    output[2] = interval;
    output[3] = argc;

    for(; i < g_argc; ++i)
    {
        output[4 + i] = argv[i];
    }

    output[g_argc + TOTAL_INPUT_TO_EXCPECT - 1] = NULL;

    return output;
}

/*****************************API Functions************************************/

wd_status_t StartWD(size_t threshold, size_t interval, int argc, char** argv)
{
    char threshold_buffer[BUFSIZE];
    char interval_buffer[BUFSIZE];
    char argc_buffer[BUFSIZE];
    char pid_buffer[BUFSIZE];
    char** exec_args = NULL;
    sem_t* sem;

    assert(threshold != 0);
    assert(interval != 0);

    sem = sem_open(SEM_NAME, O_CREAT, 0666, 0);

    if(SEM_FAILED == sem)
    {
        return WD_FAILED;
    }

    g_threshold = threshold;
    g_interval = interval;
    g_argc = argc;
    exec_args = CreateExecArgsInput(threshold_buffer, interval_buffer,
                                                            argc_buffer, argv);

    if(!exec_args)
    {
        CleanResources(sem, exec_args);
        return WD_FAILED;
    }

    pid = fork();

    if(-1 == pid)
    {
        CleanResources(sem, exec_args);
        return WD_FAILED;
    }

    if(0 == pid)
    {
        if(-1 == execvp(EXEC_FILE_RUN, exec_args))
        {
            CleanResources(sem, exec_args);
            return WD_FAILED;
        }
    }
    
    sprintf(pid_buffer, "%d" ,pid);
    
    if(-1 == setenv(ENV_VAR_NAME, pid_buffer, 1))
    {
        CleanResources(sem, exec_args);
        return WD_FAILED;
    }

    sem_wait(sem);

    if(0 != pthread_create(&thread, NULL, ThreadStart, argv))
    {
        kill(pid, SIGUSR2);
        CleanResources(sem, exec_args);
        return WD_FAILED;
    }

    sem_wait(sem);
    CleanResources(sem, exec_args);

    return WD_SUCCESS;
}

void StopWD(void)
{
    kill(pid, SIGUSR2);
    waitpid(pid, NULL, 0);
    raise(SIGUSR2);
    pthread_join(thread, NULL);
    unsetenv(ENV_VAR_NAME);
}