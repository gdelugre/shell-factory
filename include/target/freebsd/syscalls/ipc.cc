#ifndef SYSCALL_IPC_H_
#define SYSCALL_IPC_H_

#include <unistd.h>

namespace Syscall {

    SYSTEM_CALL int pipe(int pipefd[2]);

    SYSTEM_CALL
    int pipe(int pipefd[2])
    {
        return DO_SYSCALL(pipe, pipefd);
    }
}

#endif
