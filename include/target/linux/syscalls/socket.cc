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
    SYSTEM_CALL ssize_t recv(int, void *, size_t, int);
    SYSTEM_CALL ssize_t recvfrom(int, void *, size_t, int, struct sockaddr *, socklen_t *);
    SYSTEM_CALL ssize_t recvmsg(int, struct msghdr *, int);
    SYSTEM_CALL ssize_t send(int, const void *, size_t, int);
    SYSTEM_CALL ssize_t sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
    SYSTEM_CALL ssize_t sendmsg(int, const struct msghdr *, int);

    #if SYSCALL_EXISTS(socketcall)
    SYSTEM_CALL
    int socketcall(int num, long *args)
    {
        return DO_SYSCALL(socketcall, num, args);
    }
    #endif

    SYSTEM_CALL
    int socket(int domain, int type, int protocol)
    {
        #if SYSCALL_EXISTS(socket)
        return DO_SYSCALL(socket, domain, type, protocol);
        #else
        long args[] = { domain, type, protocol };
        return socketcall(SYS_SOCKET, args);
        #endif
    }

    SYSTEM_CALL
    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
    {
        #if SYSCALL_EXISTS(getsockopt)
        return DO_SYSCALL(getsockopt, sockfd, level, optname, optval, optlen);
        #else
        long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
        return socketcall(SYS_GETSOCKOPT, args);
        #endif
    }

    SYSTEM_CALL
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    {
        #if SYSCALL_EXISTS(setsockopt)
        return DO_SYSCALL(setsockopt, sockfd, level, optname, optval, optlen);
        #else
        long args[] = { sockfd, level, optname, (long) optval, (long) optlen };
        return socketcall(SYS_SETSOCKOPT, args);
        #endif
    }

    SYSTEM_CALL
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        #if SYSCALL_EXISTS(connect)
        return DO_SYSCALL(connect, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_CONNECT, args);
        #endif
    }

    SYSTEM_CALL
    int listen(int socket, int backlog)
    {
        #if SYSCALL_EXISTS(listen)
        return DO_SYSCALL(listen, socket, backlog);
        #else
        long args[] = { socket, backlog };
        return socketcall(SYS_LISTEN, args);
        #endif
    }

    SYSTEM_CALL
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        #if SYSCALL_EXISTS(bind)
        return DO_SYSCALL(bind, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_BIND, args);
        #endif
    }

    SYSTEM_CALL
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    {
        #if SYSCALL_EXISTS(accept)
        return DO_SYSCALL(accept, sockfd, addr, addrlen);
        #else
        long args[] = { sockfd, (long) addr, (long) addrlen };
        return socketcall(SYS_ACCEPT, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
    {
        #if SYSCALL_EXISTS(recvmsg)
        return DO_SYSCALL(recvmsg, sockfd, msg, flags);
        #else
        long args[] = { sockfd, (long) msg, flags };
        return socketcall(SYS_RECVMSG, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen)
    {
        #if SYSCALL_EXISTS(recvfrom)
        return DO_SYSCALL(recvfrom, sockfd, buf, len, flags, src_addr, addrlen);
        #else
        long args[] = { sockfd, (long) buf, (long) len, flags, (long) src_addr, (long) addrlen };
        return socketcall(SYS_RECVFROM, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t recv(int sockfd, void *buf, size_t len, int flags)
    {
        #if SYSCALL_EXISTS(recvfrom)
        return recvfrom(sockfd, buf, len, flags, nullptr, 0);
        #else
        long args[] = { sockfd, (long) buf, (long) len, flags };
        return socketcall(SYS_RECV, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
    {
        #if SYSCALL_EXISTS(sendmsg)
        return DO_SYSCALL(sendmsg, sockfd, msg, flags);
        #else
        long args[] = { sockfd, (long) msg, flags };
        return socketcall(SYS_SENDMSG, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen)
    {
        #if SYSCALL_EXISTS(sendto)
        return DO_SYSCALL(sendto, sockfd, buf, len, flags, dest_addr, addrlen);
        #else
        long args[] = { sockfd, (long) buf, (long) len, flags, (long) dest_addr, (long) addrlen };
        return socketcall(SYS_SENDTO, args);
        #endif
    }

    SYSTEM_CALL
    ssize_t send(int sockfd, const void *buf, size_t len, int flags)
    {
        #if SYSCALL_EXISTS(sendto)
        return sendto(sockfd, buf, len, flags, nullptr, 0);
        #else
        long args[] = { sockfd, (long) buf, (long) len, flags };
        return socketcall(SYS_SEND, args);
        #endif
    }
}

#endif
