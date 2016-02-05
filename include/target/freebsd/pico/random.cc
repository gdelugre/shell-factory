#ifndef PICOLIB_RANDOM_IMPL_H_
#define PICOLIB_RANDOM_IMPL_H_

namespace Pico {

    CONSTRUCTOR
    SecureRandom::SecureRandom()
    {
        pool = Filesystem::File::open("/dev/urandom").file_desc();
    }

    DESTRUCTOR
    SecureRandom::~SecureRandom()
    {
        Syscall::close(pool);
    }

    template <typename T>
    METHOD
    size_t SecureRandom::read(T& value)
    {
        size_t remaining = sizeof(T);
        char *ptr = static_cast<char *>(value);

        while ( remaining )
        {
            ssize_t ret = Syscall::read(pool, ptr, remaining);
            if ( Target::is_error(ret) )
                continue;

            remaining -= ret;
            ptr += ret;
        }

        return sizeof(T);
    }
}

#endif
