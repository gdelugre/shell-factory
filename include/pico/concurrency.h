#ifndef PICOLIB_CONCURRENCY_H_
#define PICOLIB_CONCURRENCY_H_

#include <cstdint>

/* 
 * XXX: Locking of statically declared variables in functions.
 *
 * Synchronization primitives are not implemented yet.
 * This is a non thread-safe implementation.
 */
extern "C" {
    using guard_type = uint64_t;

    EXPORT_ABI_FUNCTION int __cxa_guard_acquire(guard_type* guard)
    { 
        char *initialized = reinterpret_cast<char *>(guard);
        int ret = *initialized == 0;

        *initialized = 1;
        return ret; 
    }

    EXPORT_ABI_FUNCTION void __cxa_guard_release(guard_type* guard) {}
}

#endif
