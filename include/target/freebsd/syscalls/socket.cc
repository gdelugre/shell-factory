#ifndef SYSCALL_SOCKET_H_
#define SYSCALL_SOCKET_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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

    SYSTEM_CALL
    int socket(int domain, int type, int protocol)
    {
        return DO_SYSCALL(socket, domain, type, protocol);
    }

    SYSTEM_CALL
    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen)
    {
        return DO_SYSCALL(getsockopt, sockfd, level, optname, optval, optlen);
    }

    SYSTEM_CALL
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    {
        return DO_SYSCALL(setsockopt, sockfd, level, optname, optval, optlen);
    }

    SYSTEM_CALL
    int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        return DO_SYSCALL(connect, sockfd, addr, addrlen);
    }

    SYSTEM_CALL
    int listen(int socket, int backlog)
    {
        return DO_SYSCALL(listen, socket, backlog);
    }

    SYSTEM_CALL
    int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
    {
        return DO_SYSCALL(bind, sockfd, addr, addrlen);
    }

    SYSTEM_CALL
    int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
    {
        return DO_SYSCALL(accept, sockfd, addr, addrlen);
    }

    SYSTEM_CALL
    ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags)
    {
        return DO_SYSCALL(recvmsg, sockfd, msg, flags);
    }

    SYSTEM_CALL
    ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                     struct sockaddr *src_addr, socklen_t *addrlen)
    {
        return DO_SYSCALL(recvfrom, sockfd, buf, len, flags, src_addr, addrlen);
    }

    SYSTEM_CALL
    ssize_t recv(int sockfd, void *buf, size_t len, int flags)
    {
        return recvfrom(sockfd, buf, len, flags, nullptr, 0);
    }

    SYSTEM_CALL
    ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
    {
        return DO_SYSCALL(sendmsg, sockfd, msg, flags);
    }

    SYSTEM_CALL
    ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
                   const struct sockaddr *dest_addr, socklen_t addrlen)
    {
        return DO_SYSCALL(sendto, sockfd, buf, len, flags, dest_addr, addrlen);
    }

    SYSTEM_CALL
    ssize_t send(int sockfd, const void *buf, size_t len, int flags)
    {
        return sendto(sockfd, buf, len, flags, nullptr, 0);
    }
}

#endif
