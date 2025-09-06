#include <stdio.h>
#include <unistd.h>

#include "watchdog.h"

#define THRESHOLD (3)
#define INTERVAL (3)
#define LOOPS (0xCAFEBABE)

int main(int argc, char* argv[])
{
    FILE* fd = fopen("MrMeeseeks.txt", "a");
    size_t i = 0;
    
    StartWD(THRESHOLD, INTERVAL, argc, argv);
    printf("Running now\n");

    fprintf(fd, "Hi, Im Mr Meeseeks!!\n");

    for(; i < LOOPS; ++i)
    {
        if(i % 100000 == 0)
        {
            fprintf(fd, "Hi Mr Meeseeks, Im Mr Meeseeks!!\n");
        }   
    }

    fprintf(fd, "Kill Jerry!!!!\n");

    fclose(fd);

    StopWD();

    return 0;
}