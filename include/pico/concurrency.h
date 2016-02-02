#ifndef PICOLIB_CONCURRENCY_H_
#define PICOLIB_CONCURRENCY_H_

namespace Pico {

    namespace Memory {

        FUNCTION
        void barrier()
        {
            __sync_synchronize();
        }
    }

    template <typename T>
    class Atomic;

    template <typename T>
    class Atomic<T *>
    {
        public:
            CONSTRUCTOR Atomic(T *ptr) : ptr(ptr) {}
            METHOD bool compare_exchange(T expected, T desired) {
                return __sync_bool_compare_and_swap(ptr, expected, desired);
            }
            METHOD T operator ++() {
                return __sync_add_and_fetch(ptr, 1);
            }

            METHOD T operator ++(int) {
                return __sync_fetch_and_add(ptr, 1);
            }
            METHOD T operator --() {
                return __sync_sub_and_fetch(ptr, 1);
            }

            METHOD T operator --(int) {
                return __sync_fetch_and_sub(ptr, 1);
            }

            METHOD T& operator *() {
                return *ptr;
            }

        private:
            T *ptr;
    };

    template <typename T>
    class Atomic
    {
        public:
            CONSTRUCTOR Atomic(T init_val) : value(init_val) {}
            METHOD bool compare_exchange(T expected, T desired) {
                return __sync_bool_compare_and_swap(&value, expected, desired);
            }
            METHOD T operator ++() {
                return __sync_add_and_fetch(&value, 1);
            }

            METHOD T operator ++(int) {
                return __sync_fetch_and_add(&value, 1);
            }
            METHOD T operator --() {
                return __sync_sub_and_fetch(&value, 1);
            }

            METHOD T operator --(int) {
                return __sync_fetch_and_sub(&value, 1);
            }

            METHOD T& operator *() {
                return value;
            }

        private:
            T value;
    };

    class Mutex
    {
        using mutex_t = Target::mutex_t;

        public:
            CONSTRUCTOR Mutex();
            METHOD int lock();
            METHOD int lock(struct timespec timeout);
            METHOD int try_lock();
            METHOD int unlock();

        private:
            Atomic<unsigned> nr_waiters = 0;
            mutex_t mutex_obj;
    };

    class CriticalSection
    {
        public:
            CONSTRUCTOR CriticalSection(Mutex& mutex) : mutex(mutex)
            {
                mutex.lock();
            }

            DESTRUCTOR ~CriticalSection()
            {
                mutex.unlock();
            }

        private:
            Mutex& mutex;
    };
}

/* 
 * Locking of statically declared variables in functions.
 */
extern "C" {
    using guard_type = char;

    EXPORT_ABI_FUNCTION int __cxa_guard_acquire(guard_type* guard)
    { 
        Pico::Atomic<guard_type *> atom(guard);
        return atom.compare_exchange(0, 1);
    }

    EXPORT_ABI_FUNCTION void __cxa_guard_release(UNUSED guard_type* guard) {}
}

#endif
