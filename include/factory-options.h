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

namespace Options {

    constexpr static bool fork_on_accept = OPT_FORK_ON_ACCEPT;
    constexpr static bool reuse_addr = OPT_REUSE_ADDR;
}

#undef OPT_FORK_ON_ACCEPT
#undef OPT_REUSE_ADDR
#undef FORK_ON_ACCEPT
#undef REUSE_ADDR
#endif
