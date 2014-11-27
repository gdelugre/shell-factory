#ifndef _PROCESS_HELPER_H
#define _PROCESS_HELPER_H

#include <factory.h>
#include <unistd.h>

static inline void
__exit(int status)
{
    INTERNAL_SYSCALL(exit,, 1, status);
}

static inline void
_exit_group(int status)
{
    INTERNAL_SYSCALL(exit_group,, 1, status);
}

#endif
