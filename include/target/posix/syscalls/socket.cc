#ifndef POSIX_SYSCALL_SOCKET_H_
#define POSIX_SYSCALL_SOCKET_H_

#include <sys/socket.h>

namespace Syscall {

    SYSTEM_CALL int socket(int, int, int);
    SYSTEM_CALL int socketpair(int, int, int, int[2]);
    SYSTEM_CALL int shutdown(int, int);
    SYSTEM_CALL int getsockopt(int, int, int , void *__restrict__, socklen_t *__restrict__);
    SYSTEM_CALL int setsockopt(int, int, int, const void *, socklen_t);
    SYSTEM_CALL int getsockname(int, struct sockaddr *__restrict__, socklen_t *__restrict__);
    SYSTEM_CALL int getpeername(int, struct sockaddr *__restrict__, socklen_t *__restrict__);
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

    #if SYSCALL_EXISTS(socket)
    SYSTEM_CALL
    int socket(int domain, int type, int protocol)
    {
        return DO_SYSCALL(socket, domain, type, protocol);
    }
    #endif

    #if SYSCALL_EXISTS(socketpair)
    SYSTEM_CALL
    int socketpair(int domain, int type, int protocol, int sv[2])
    {
        return DO_SYSCALL(socketpair, domain, type, protocol, sv);
    }
    #endif

    #if SYSCALL_EXISTS(shutdown)
    SYSTEM_CALL
    int shutdown(int s, int how)
    {
        return DO_SYSCALL(shutdown, s, how);
    }
    #endif

    #if SYSCALL_EXISTS(getsockopt)
    SYSTEM_CALL
    int getsockopt(int sockfd, int level, int optname, void *__restrict__ optval, socklen_t *__restrict__ optlen)
    {
        return DO_SYSCALL(getsockopt, sockfd, level, optname, optval, optlen);
    }
    #endif

    #if SYSCALL_EXISTS(setsockopt)
    SYSTEM_CALL
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    {
        return DO_SYSCALL(setsockopt, sockfd, level, optname, optval, optlen);
    }
    #endif

    #if SYSCALL_EXISTS(getsockname)
    SYSTEM_CALL
    int getsockname(int s, struct sockaddr *__restrict__ name, socklen_t *__restrict__ namelen)
    {
        return DO_SYSCALL(getsockname, s, name, namelen);
    }
    #endif

    #if SYSCALL_EXISTS(getpeername)
    SYSTEM_CALL
    int getpeername(int s, struct sockaddr *__restrict__ name, socklen_t *__restrict__ namelen)
    {
        return DO_SYSCALL(getpeername, s, name, namelen);
    }
    #endif

    #if SYSCALL_EXISTS(connect)
    SYSTEM_CALL
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        return DO_SYSCALL(connect, sockfd, addr, addrlen);
    }
    #endif

    #if SYSCALL_EXISTS(listen)
    SYSTEM_CALL
    int listen(int socket, int backlog)
    {
        return DO_SYSCALL(listen, socket, backlog);
    }
    #endif

    #if SYSCALL_EXISTS(bind)
    SYSTEM_CALL
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        return DO_SYSCALL(bind, sockfd, addr, addrlen);
    }
    #endif

    #if SYSCALL_EXISTS(accept)
    SYSTEM_CALL
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    {
        return DO_SYSCALL(accept, sockfd, addr, addrlen);
    }
    #endif

    #if SYSCALL_EXISTS(recvmsg)
    SYSTEM_CALL
    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
    {
        return DO_SYSCALL(recvmsg, sockfd, msg, flags);
    }
    #endif

    #if SYSCALL_EXISTS(recvfrom)
    SYSTEM_CALL
    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen)
    {
        return DO_SYSCALL(recvfrom, sockfd, buf, len, flags, src_addr, addrlen);
    }
    #endif

    SYSTEM_CALL
    ssize_t recv(int s, void *buf, size_t len, int flags)
    {
        #if SYSCALL_EXISTS(recv)
        return DO_SYSCALL(recv, s, buf, len, flags);
        #else
        return recvfrom(s, buf, len, flags, nullptr, nullptr);
        #endif
    }

    #if SYSCALL_EXISTS(sendmsg)
    SYSTEM_CALL
    ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
    {
        return DO_SYSCALL(sendmsg, sockfd, msg, flags);
    }
    #endif

    #if SYSCALL_EXISTS(sendto)
    SYSTEM_CALL
    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen)
    {
        return DO_SYSCALL(sendto, sockfd, buf, len, flags, dest_addr, addrlen);
    }
    #endif

    SYSTEM_CALL
    ssize_t send(int s, const void *msg, size_t len, int flags)
    {
        #if SYSCALL_EXISTS(send)
        return DO_SYSCALL(send, s, msg, len, flags);
        #else
        return sendto(s, msg, len, flags, nullptr, 0);
        #endif
    }
}

#endif
