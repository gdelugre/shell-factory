#ifndef SYSCALL_SOCKET_H_ 
#define SYSCALL_SOCKET_H_

#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/net.h>

/* 
 * Syscalls defined in this file. 
 */
namespace Syscall {

    SYSTEM_CALL int socket(int, int, int);
    SYSTEM_CALL int getsockopt(int, int, int , void *, socklen_t *);
    SYSTEM_CALL int setsockopt(int, int, int, const void *, socklen_t);
    SYSTEM_CALL int connect(int, const struct sockaddr *, socklen_t);
    SYSTEM_CALL int listen(int, int);
    SYSTEM_CALL int bind(int, const struct sockaddr *, socklen_t);
    SYSTEM_CALL int accept(int, struct sockaddr *, socklen_t *);

    #if SYSCALL_EXISTS(socketcall)
    SYSTEM_CALL
    int socketcall(int num, long *args)
    {
        return DO_SYSCALL(socketcall, 2, num, args);
    }
    #endif

    SYSTEM_CALL
    int socket(int domain, int type, int protocol)
    {
        #if SYSCALL_EXISTS(socket)
        return DO_SYSCALL(socket, 3, domain, type, protocol);
        #else
        long args[] = { domain, type, protocol };
        return socketcall(SYS_SOCKET, args);
        #endif
    }

    SYSTEM_CALL
    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
    {
        #if SYSCALL_EXISTS(getsockopt)
        return DO_SYSCALL(getsockopt, 5, sockfd, level, optname, optval, optlen);
        #else
        long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
        return socketcall(SYS_GETSOCKOPT, args);
        #endif
    }

    SYSTEM_CALL
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    {
        #if SYSCALL_EXISTS(setsockopt)
        return DO_SYSCALL(setsockopt, 5, sockfd, level, optname, optval, optlen);
        #else
        long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
        return socketcall(SYS_SETSOCKOPT, args);
        #endif
    }

    SYSTEM_CALL
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        #if SYSCALL_EXISTS(connect)
        return DO_SYSCALL(connect, 3, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_CONNECT, args);
        #endif
    }

    SYSTEM_CALL
    int listen(int socket, int backlog)
    {
        #if SYSCALL_EXISTS(listen)
        return DO_SYSCALL(listen, 2, socket, backlog);
        #else
        long args[] = { socket, backlog };
        return socketcall(SYS_LISTEN, args);
        #endif
    }

    SYSTEM_CALL
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        #if SYSCALL_EXISTS(bind)
        return DO_SYSCALL(bind, 3, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_BIND, args);
        #endif
    }

    SYSTEM_CALL
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    {
        #if SYSCALL_EXISTS(accept)
        return DO_SYSCALL(accept, 3, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_ACCEPT, args);
        #endif
    }
}

#endif
