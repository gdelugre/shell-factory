#ifndef PICOLIB_SECURITY_IMPL_H_
#define PICOLIB_SECURITY_IMPL_H_

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
