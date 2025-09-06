#include <stdlib.h>      /* atoi */
#include <stdio.h>       /* fprintf */

#include "inner_watchdog.h"

int main(int argc, char* argv[])
{
    (void)argc;

    if(-1 == RunWD((size_t)atoi(argv[1]), (size_t)atoi(argv[2]), atoi(argv[3]),
                                                            argv + 4, SERVER))
    {
        fprintf(stderr, "Process creation failed\n");
        return -1;
    }

    return 0;
}