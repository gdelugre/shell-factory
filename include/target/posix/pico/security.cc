#ifndef POSIX_PICO_SECURITY_H_
#define POSIX_PICO_SECURITY_H_

namespace Pico {

    User User::current()
    {
        return User( Syscall::getuid() );
    }

    Group Group::current()
    {
        return Group( Syscall::getgid() );
    }
}

#endif
