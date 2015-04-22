#ifndef __UDP_CLI_H_
#define __UDP_CLI_H_

#include <stdint.h>
#include <sys/types.h>                                         
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#include <string>

class UDP { 
public:
    UDP(const std::string& server_ip, uint16_t server_port):sock_fd_(-1), server_ip_(server_ip), server_port_(server_port) {}
    ~UDP() {
        if (sock_fd_>=0) {
            close(sock_fd_);
        }  
    }

    bool CreateSocket() { 
        sock_fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_fd_ == -1) {
            perror("create socket fail:");
            return false;
        }
        
        //struct sockaddr_in src;
        //src.sin_family      = AF_INET;
        //src.sin_addr.s_addr = inet_addr("0.0.0.0");
        //src.sin_port        = htons(0);
        //bzero(src.sin_zero,sizeof(src.sin_zero));

        //if(bind(sock_fd_, (sockaddr *)&src, sizeof(sockaddr))==-1) {
        //    perror("bind sockfd fail:");
        //    return false;
        //}
       
        const int recv_len = 64000; 
        setsockopt(sock_fd_, SOL_SOCKET, SO_RCVBUF, (const char*)&recv_len, sizeof(int));
        const int send_len = 64000; 
        setsockopt(sock_fd_, SOL_SOCKET, SO_SNDBUF, (const char*)&send_len, sizeof(int));

        return true; 
    }
    
    bool SendAndRecvData(const std::string& send_data, std::string& recv_data) { 
        struct sockaddr_in dst;
        dst.sin_family      = AF_INET;                                               
        dst.sin_addr.s_addr = inet_addr(server_ip_.c_str());
        dst.sin_port        = htons(server_port_);
        bzero(dst.sin_zero, sizeof(dst.sin_zero));

        if(sendto(sock_fd_, send_data.data(), send_data.size(), 0, (sockaddr *)&dst, sizeof(sockaddr)) < 0) {        
            printf("send data fail: size=%ld, %s\n", send_data.size(), strerror(errno));
            return false;
        } 

        socklen_t socklen = sizeof(sockaddr);
        recv_data.resize(100000);
        ssize_t msglen = recvfrom(sock_fd_, &(recv_data[0]), 100000, 0, (sockaddr*)&dst, &socklen);
        if( msglen < 0 ) {
            perror("recv data fail:");
            return false;
        }
        recv_data.resize(msglen);
        return true;
    }

private:
    int         sock_fd_;
    std::string server_ip_;
    uint16_t    server_port_; 
};

#endif


