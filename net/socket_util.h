#ifndef __SOCKET_UTIL_H
#define __SOCKET_UTIL_H

#define NS_BEG_NET namespace net {
#define NS_END_NET } // end of namespace net 


NS_BEG_NET

int socket_accept(int listenfd, struct sockaddr *addr);

ssize_t socket_sendn(int sockfd, const char* buffer, size_t buflen);

int socket_readn(int sockfd, size_t data_len, char* recv_buf, size_t buf_len); 

/*
 * keep_idle      //首次探测开始前的tcp无数据收发空闲时间
 * keep_interval  //每次探测的间隔时间
 * keep_count     //探测次数
 * */
int socket_keepalive(int sockfd, int keep_idle=5, int keep_interval=3, int keep_count);

NS_END_NET


#endif
