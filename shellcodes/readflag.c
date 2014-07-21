#include <factory.h>
#include "helpers/fs.c"
#include "helpers/socket.c"

#if !defined(FLAG_PATH)
#error "FLAG_PATH variable is not defined."
#endif

SHELLCODE_ENTRY {

    int flag_fd = _open(TO_STRING(FLAG_PATH), O_RDONLY);
    struct channel chan = get_communication_channel();
    char c;

    while ( _read(chan.rx, &c, sizeof(c)) > 0 )
    {
        _write(chan.tx, &c, sizeof(c));
    }

} SHELLCODE_END
