#ifndef __INNER_WATCHDOG_H__
#define __INNER_WATCHDOG_H__

#include <stddef.h>         /* size_t */

#include "heap_scheduler.h"

#define SEM_NAME ("/WatchDog")
#define ENV_VAR_NAME ("WD_PID")
#define BUFSIZE (64)

#ifndef NDEBUG

#define EXEC_FILE_RUN ("./debug/wd.out")

#else

#define EXEC_FILE_RUN ("./release/wd.out")

#endif

typedef enum wd_type {
    CLIENT,
    SERVER
} wd_type_t;


int RunWD(size_t threshold, size_t interval, int argc, char** argv,
                                                            wd_type_t location);

#endif  /*__WATCHDOG_H__*/