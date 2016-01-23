#ifndef PICOLIB_RANDOM_IMPL_H_
#define PICOLIB_RANDOM_IMPL_H_

namespace Pico {

    #if SYSCALL_EXISTS(getrandom)

    CONSTRUCTOR SecureRandom::SecureRandom() {}
    DESTRUCTOR SecureRandom::~SecureRandom() {}

    template <typename T>
    size_t SecureRandom::read(T& value)
    {
        size_t remaining = sizeof(T);
        char *ptr = static_cast<char *>(value);

        while ( remaining )
        {
            ssize_t ret = Syscall::getrandom(ptr, remaining, 0);
            if ( ret < 0 )
                continue;

            remaining -= ret;
            ptr += ret;
        }

        return sizeof(T);
    }

    #else /* No getrandom syscall, rely on /dev/urandom */

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
            if ( ret < 0 )
                continue;

            remaining -= ret;
            ptr += ret;
        }

        return sizeof(T);
    }

    #endif
}

#endif
