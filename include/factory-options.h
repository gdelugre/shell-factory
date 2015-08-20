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

namespace Options {

    constexpr static bool fork_on_accept = OPT_FORK_ON_ACCEPT;
    constexpr static bool reuse_addr = OPT_REUSE_ADDR;
    constexpr static bool use_builtins = OPT_USE_BUILTINS;

    static_assert(fork_on_accept == false, "Fork on accept is not supported yet.");
}

#undef OPT_FORK_ON_ACCEPT
#undef OPT_REUSE_ADDR
#undef OPT_USE_BUILTINS
#undef FORK_ON_ACCEPT
#undef REUSE_ADDR
#undef NO_BUILTIN
#endif
