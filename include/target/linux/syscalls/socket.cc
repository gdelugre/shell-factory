#ifndef SYSCALL_SOCKET_H_
#define SYSCALL_SOCKET_H_

#include <netinet/in.h>
#include <netdb.h>
#include <linux/net.h>

/*
 * This file defines Linux specific system calls (i.e. not POSIX).
 */
namespace Syscall {

    //
    // Some architectures will use a common socketcall entry point for all operations.
    // Redefine the common POSIX socket functions to use socketcall if necessary.
    //
    #if SYSCALL_EXISTS(socketcall)

        SYSTEM_CALL int socketcall(int num, long *args);

        SYSTEM_CALL
        int socketcall(int num, long *args)
        {
            return DO_SYSCALL(socketcall, num, args);
        }

        #if !SYSCALL_EXISTS(socket)
        SYSTEM_CALL
        int socket(int domain, int type, int protocol)
        {
            long args[] = { domain, type, protocol };
            return socketcall(SYS_SOCKET, args);
        }
        #endif

        #if !SYSCALL_EXISTS(socketpair)
        SYSTEM_CALL
        int socketpair(int domain, int type, int protocol, int sv[2])
        {
            long args[] = { domain, type, protocol, (long) sv };
            return socketcall(SYS_SOCKETPAIR, args); 
        }
        #endif

        #if !SYSCALL_EXISTS(shutdown)
        SYSTEM_CALL
        int shutdown(int s, int how)
        {
            long args[] = { s, how };
            return socketcall(SYS_SHUTDOWN, args);
        }
        #endif

        #if !SYSCALL_EXISTS(getsockopt)
        SYSTEM_CALL
        int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
        {
            long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
            return socketcall(SYS_GETSOCKOPT, args);
        }
        #endif

        #if !SYSCALL_EXISTS(setsockopt)
        SYSTEM_CALL
        int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
        {
            long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
            return socketcall(SYS_SETSOCKOPT, args);
        }
        #endif

        #if !SYSCALL_EXISTS(getsockname)
        SYSTEM_CALL
        int getsockname(int sockfd, struct sockaddr *__restrict addr, socklen_t *__restrict__ addrlen)
        {
            long args[] = { sockfd, (long) addr, (long) addrlen };
            return socketcall(SYS_GETSOCKNAME, args);
        }
        #endif

        #if !SYSCALL_EXISTS(getpeername)
        SYSTEM_CALL
        int getpeername(int sockfd, struct sockaddr *__restrict__ addr, socklen_t *__restrict__ addrlen)
        {
            long args[] = { sockfd, (long) addr, (long) addrlen };
            return socketcall(SYS_GETPEERNAME, sockfd, addr, addrlen);
        }
        #endif

        #if !SYSCALL_EXISTS(connect)
        SYSTEM_CALL
        int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        {
            long args[] = { sockfd, (long) addr, (long) addrlen };
            return socketcall(SYS_CONNECT, args);
        }
        #endif

        #if !SYSCALL_EXISTS(listen)
        SYSTEM_CALL
        int listen(int socket, int backlog)
        {
            long args[] = { socket, backlog };
            return socketcall(SYS_LISTEN, args);
        }
        #endif

        #if !SYSCALL_EXISTS(bind)
        SYSTEM_CALL
        int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
        {
            long args[] = { sockfd, (long) addr, (long) addrlen };
            return socketcall(SYS_BIND, args);
        }
        #endif

        #if !SYSCALL_EXISTS(accept)
        SYSTEM_CALL
        int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
        {
            long args[] = { sockfd, (long) addr, (long) addrlen };
            return socketcall(SYS_ACCEPT, args);
        }
        #endif

        #if !SYSCALL_EXISTS(recvmsg)
        SYSTEM_CALL
        ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
        {
            long args[] = { sockfd, (long) msg, flags };
            return socketcall(SYS_RECVMSG, args);
        }
        #endif

        #if !SYSCALL_EXISTS(recvfrom)
        SYSTEM_CALL
        ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                         struct sockaddr *src_addr, socklen_t *addrlen)
        {
            long args[] = { sockfd, (long) buf, (long) len, flags, (long) src_addr, (long) addrlen };
            return socketcall(SYS_RECVFROM, args);
        }
        #endif

        #if !SYSCALL_EXISTS(sendmsg)
        SYSTEM_CALL
        ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
        {
            long args[] = { sockfd, (long) msg, flags };
            return socketcall(SYS_SENDMSG, args);
        }
        #endif

        #if !SYSCALL_EXISTS(sendto)
        SYSTEM_CALL
        ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                       const struct sockaddr *dest_addr, socklen_t addrlen)
        {
            long args[] = { sockfd, (long) buf, (long) len, flags, (long) dest_addr, (long) addrlen };
            return socketcall(SYS_SENDTO, args);
        }
        #endif

    #endif // SYSCALL_EXISTS(socketcall)
}

#endif
