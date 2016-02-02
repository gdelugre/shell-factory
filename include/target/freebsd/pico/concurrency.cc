#ifndef PICOLIB_CONCURRENCY_IMPL_H_
#define PICOLIB_CONCURRENCY_IMPL_H_

#include <sys/umtx.h>

namespace Pico {

    static constexpr int UMTX_FREE     = 0;
    static constexpr int UMTX_OWNED    = 1;

    CONSTRUCTOR
    Mutex::Mutex() : mutex_obj(UMTX_FREE) {}

    METHOD
    int Mutex::try_lock()
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        return guard.compare_exchange(UMTX_FREE, UMTX_OWNED) ? 0 : -1;
    }

    METHOD
    int Mutex::lock()
    {
        Atomic<mutex_t *> guard(&mutex_obj);

        while ( try_lock() != 0 ) {
            nr_waiters++;
            int ret = Syscall::_umtx_op(&mutex_obj, UMTX_OP_WAIT, UMTX_OWNED, nullptr, nullptr);
            nr_waiters--;

            if ( Target::is_error(ret) ) {
                if (ret == EINTR)
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
            int ret = Syscall::_umtx_op(&mutex_obj, UMTX_OP_WAIT, UMTX_OWNED, nullptr, &timeout);
            nr_waiters--;

            if ( Target::is_error(ret) ) {
                if (ret == EINTR)
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

        if ( guard.compare_exchange(UMTX_OWNED, UMTX_FREE) ) {
            if ( *nr_waiters > 0 ) {
                int ret = Syscall::_umtx_op(&mutex_obj, UMTX_OP_WAKE, 1, nullptr, nullptr);
                if ( Target::is_error(ret) )
                    return -1;
            }
        }

        return 0;
    } 
}

#endif
