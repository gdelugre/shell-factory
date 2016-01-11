#ifndef FACTORY_OPTIONS_H_
#define FACTORY_OPTIONS_H_

#if !defined(FORK_ON_ACCEPT) || (FORK_ON_ACCEPT == 0)
#define OPT_FORK_ON_ACCEPT false
#else
#define OPT_FORK_ON_ACCEPT true
#endif

#if !defined(REUSE_ADDR) || (REUSE_ADDR == 0)
#define OPT_REUSE_ADDR false
#else
#define OPT_REUSE_ADDR true
#endif

#if !defined(NO_BUILTIN) || (NO_BUILTIN == 0)
#define OPT_USE_BUILTINS true
#else
#define OPT_USE_BUILTINS false
#endif

#if defined(HEAP_BASE)
#define OPT_HEAP_BASE (void *) HEAP_BASE
#else
#define OPT_HEAP_BASE (void *) &eend
#endif

#if defined(HEAP_SIZE)
#define OPT_HEAP_SIZE HEAP_SIZE
#else
#define OPT_HEAP_SIZE 64 * 1024
#endif

namespace Options {

    constexpr static bool fork_on_accept = OPT_FORK_ON_ACCEPT;
    constexpr static bool reuse_addr = OPT_REUSE_ADDR;
    constexpr static bool use_builtins = OPT_USE_BUILTINS;
    static void *heap_address = OPT_HEAP_BASE;
    constexpr static size_t heap_size = OPT_HEAP_SIZE;
}

#undef OPT_FORK_ON_ACCEPT
#undef OPT_REUSE_ADDR
#undef OPT_USE_BUILTINS
#undef FORK_ON_ACCEPT
#undef REUSE_ADDR
#undef NO_BUILTIN
#endif
