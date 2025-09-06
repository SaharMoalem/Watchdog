#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

#include <stddef.h>         /* size_t */

typedef enum wd_status {
    WD_SUCCESS,
    WD_FAILED
} wd_status_t;

wd_status_t StartWD(size_t threshold, size_t interval, int argc, char** argv);

void StopWD(void);

#endif  /*__WATCHDOG_H__*/