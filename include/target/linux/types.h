#ifndef LINUX_TYPES_H_
#define LINUX_TYPES_H_

#include <linux/types.h>

namespace Target {

    /* File descriptors. */
    using handle = int; 

    using user_id = uid_t;
    using group_id = gid_t;
    using thread_id = pid_t;
    using process_id = pid_t;
}

#endif
