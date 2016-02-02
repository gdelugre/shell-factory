#ifndef PICOLIB_CONCURRENCY_IMPL_H_
#define PICOLIB_CONCURRENCY_IMPL_H_

#include <linux/futex.h>
#include <sys/time.h>

namespace Pico {

    static constexpr int FUTEX_UNOWNED  = 0;
    static constexpr int FUTEX_OWNED    = 1;

    CONSTRUCTOR
    Mutex::Mutex() : mutex_obj(FUTEX_UNOWNED) {}

    METHOD
    int Mutex::try_lock()
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        return guard.compare_exchange(FUTEX_UNOWNED, FUTEX_OWNED) ? 0 : -1;
    }

    METHOD
    int Mutex::lock()
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        while ( try_lock() != 0 ) {
            nr_waiters++;
            int ret = Syscall::futex(&mutex_obj, FUTEX_WAIT, FUTEX_OWNED, nullptr, nullptr, 0);
            nr_waiters--;

            if ( Target::is_error(ret) ) {
                if (ret == -EAGAIN)
                    continue;
                return -1;
            }
        }

        return 0;
    }

    METHOD
    int Mutex::lock(struct timespec timeout)
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        while ( try_lock() != 0 ) {
            nr_waiters++;
            int ret = Syscall::futex(&mutex_obj, FUTEX_WAIT, FUTEX_OWNED, &timeout, nullptr, 0);
            nr_waiters--;

            if ( Target::is_error(ret) ) {
                if (ret == -EAGAIN)
                    continue;
                return -1;
            }
        }

        return 0;
    }

    METHOD
    int Mutex::unlock()
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        if ( guard.compare_exchange(FUTEX_OWNED, FUTEX_UNOWNED) ) {
            if ( nr_waiters > 0 ) {
                int ret = Syscall::futex(&mutex_obj, FUTEX_WAKE, 1, nullptr, nullptr, 0);
                if ( Target::is_error(ret) )
                    return -1;
            }
        }

        return 0;
    } 
}

#endif
