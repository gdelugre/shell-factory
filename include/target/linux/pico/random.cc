#ifndef PICOLIB_RANDOM_IMPL_H_
#define PICOLIB_RANDOM_IMPL_H_

namespace Pico {

    #if SYSCALL_EXISTS(getrandom)
    class SecureRandom
    {
        public:
            CONSTRUCTOR SecureRandom() = default;

            template <typename T>
            ssize_t read(T& value)
            {
                return Syscall::getrandom(&value, sizeof(value), 0);
            }
    };
    #else
    class SecureRandom
    {
        public:
            CONSTRUCTOR SecureRandom() : pool("/dev/urandom") {}
            DESTRUCTOR ~SecureRandom() {
                pool.close();
            }

            template <typename T>
            ssize_t read(T& value)
            {
                return pool.read(&value, sizeof(value));
            }

        private:
            Filesystem::File pool;
    };
    #endif
}

#endif
