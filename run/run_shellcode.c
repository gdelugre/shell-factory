#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>

#include <signal.h>

int main(int argc, char *argv[])
{
    FILE *fp;
    size_t file_sz;
    void *shellcode;

    if ( argc < 2 )
        return 1;

    fp = fopen(argv[1], "r"); 
    if ( !fp )
    {
        fprintf(stderr, "%s: %s.\n", argv[1], strerror(errno));
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    file_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET); 

    shellcode = mmap(NULL, file_sz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, 0, 0);
    if ( !shellcode )
    {
        fprintf(stderr, "mmap: %s.\n", strerror(errno)); 
        return 1;
    }

    if ( fread(shellcode, sizeof(char), file_sz, fp) != file_sz )
    {
        fprintf(stderr, "fread: %s.\n", strerror(errno));
        return 1;
    }

    fclose(fp);

    return ((int (*)(void)) shellcode)();
}
