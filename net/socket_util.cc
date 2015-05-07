#include "socket_util.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>

NS_BEG_NET

bool socket_setfd_tag(int fd, int tag, char * errbuf)
{
    int val;
    if ((val = fcntl(fd, F_GETFL, 0)) == -1) {
        snprintf(errbuf, 1024, "fcntl F_GETFL error:[%d:%s]", errno, strerror(errno));
        return false;
    }

    if (fcntl(fd, F_SETFL, val | tag) == -1) {
        snprintf(errbuf, 1024, "fcntl F_SETFL error:[%d:%s]", errno, strerror(errno));
        return false;
    }
    return true;
}

bool socket_setfd_nonblock(int fd, char * errbuf)
{
    return socket_setfd_tag(fd,O_NONBLOCK,errbuf);
}

int socket_accept(int listenfd, struct sockaddr *addr) 
{
    struct sockaddr_storage addr_storage;
    socklen_t addrlen = sizeof(addr_storage);
    int fd = accept(listenfd, (sockaddr*)&addr_storage, &addrlen);
    if (fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            return -1;   // next accpet
        }
    }

    if (addr_storage.ss_family == AF_INET) {   // ipv4 socket
        memcpy(addr, &addr_storage, addrlen);
        return fd;
    } else if (addr_storage.ss_family == AF_INET6) { // ipv6 socket
        close(fd); // not support
        return -1;
    } else { // error socket
        fprintf(stderr, "accept failed, listenfd=%d, error=%s", listenfd, strerror(errno));
        return -1;
    }
}

ssize_t socket_sendn(int sockfd, const char* buffer, size_t buflen)
{
    ssize_t len  = 0;
    size_t total = buflen;
    const char *p = buffer;

    while(true) {
        len = send(sockfd, p, total, MSG_DONTWAIT);
        if(len < 0) {
            if(errno == EINTR || errno == EAGAIN) {  // 当socket是非阻塞时,如返回此错误,表示写缓冲队列已满,在这里做延时后再重试.
                usleep(1000);
                continue;
            }
            return -1;
        }

        if((size_t)len == total) {
            return buflen;
        }

        total -= len;
        p     += len;
    }
    return (ssize_t)(p-buffer);
}

int socket_readn(int sockfd, size_t data_len, char* recv_buf, size_t buf_len)
{
    if (sockfd<0 || recv_buf == NULL || buf_len<=0) {
        return -1;
    }

    if (data_len>buf_len) {
        return -1;
    }

    int read_len = 0;
    while (read_len<data_len) {
        int ret = recv(sockfd, recv_buf+read_len, data_len-read_len, MSG_DONTWAIT);
        if (ret>0) {
            read_len += ret;
            continue;
        }

        if ( ret == 0 ) { // socket close
            return -1;
        }

        if ( ret < 0 ) {
            if ( errno == EINTR || errno == EAGAIN ) {
                usleep(1000);
                continue;
            } 
            return -1;
        }  
    }
    return read_len;
}

int socket_keepalive(int sockfd, int keep_idle/*=5*/, int keep_interval/*=3*/, int keep_count/*=2*/)
{
    int keep_alive  = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) == -1) {
        fprintf(stderr, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return -1;
    }

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keep_idle, sizeof(keep_idle)) < 0) {
        fprintf(stderr, "setsockopt TCP_KEEPIDLE: %s\n", strerror(errno));
        return -1;
    }

    int val = keep_interval/3;
    if (val==0) { val=1; }
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) {
        fprintf(stderr, "setsockopt TCP_KEEPINTVL: %s\n", strerror(errno));
        return -1;
    }

    if (setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keep_count, sizeof(keep_count)) < 0) {
        fprintf(stderr, "setsockopt TCP_KEEPCNT: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

NS_END_NET

