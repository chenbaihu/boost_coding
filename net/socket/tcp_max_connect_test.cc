#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <stdexcept>
#include <set>
using namespace std;

int main(int argc,char *argv[])
{
    if (argc<5) {
        printf("usage:%s localip serverip port connectnum\n",argv[0]);
        exit(0);
    }

    unsigned int connect_num =strtoul(argv[4],NULL,10);
    set<int> sock_set;
    unsigned int csock_failed_num=0;
    unsigned int setop_failed_num=0;
    unsigned int bind_failed_num=0;
    unsigned int conn_failed_num=0;
    unsigned int index=0;
    for (index=0; index<connect_num; index++) {
        int sock_fd=0;
        sock_fd = socket(AF_INET,SOCK_STREAM,0);    //创建socket
        if (sock_fd < 0) {
            perror("creat sock_fd fail:");
            //exit(0);
            close(sock_fd);
            csock_failed_num++;
            continue;
        }

        int option_value = 1;                       //端口复用
        int ret = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&option_value, sizeof(option_value));
        if (ret < 0) {
            perror("set sock opt fail:");
            close(sock_fd);
            //exit(0);
            close(sock_fd);
            setop_failed_num++;
            continue;
        }

        struct sockaddr_in connect_addr;           //客户端地址，主要绑定IP
        connect_addr.sin_family = AF_INET;
        connect_addr.sin_port = 0;
        connect_addr.sin_addr.s_addr = inet_addr(argv[1]);
        memset(connect_addr.sin_zero, 0, sizeof(connect_addr.sin_zero));

        //int ret;
        ret = bind(sock_fd, (struct sockaddr*)&connect_addr, sizeof(connect_addr));     //绑定客户端IP(一般不需要)
        if (ret<0) {
            perror("bind client ip failed:");
            //exit(0);
            close(sock_fd);
            bind_failed_num++;
            continue;
        }

        struct sockaddr_in server_addr;             //服务端地址
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(atoi(argv[3]));
        server_addr.sin_addr.s_addr = inet_addr(argv[2]);
        memset(server_addr.sin_zero,'\0',sizeof(server_addr.sin_zero));
        socklen_t addr_len = sizeof(struct sockaddr_in);

        ret = connect(sock_fd,(struct sockaddr*)&server_addr,addr_len);
        if (ret<0) {
            perror("connect to server failed:");
            //exit(0);
            close(sock_fd);
            conn_failed_num++;
            continue;
        }
        sock_set.insert(sock_fd);
        printf("connect succ, fd=%d\n", sock_fd);
    }

    while(true) {
        printf("connect socket num: conn=%ld \
                csock_failed_num=%u \
                setop_failed_num=%u \
                bind_failed_num=%u  \
                conn_failed_num=%u\n",
                sock_set.size(), 
                csock_failed_num, 
                setop_failed_num, 
                bind_failed_num, 
                conn_failed_num);
        sleep(10);
    }
    //close(sock_fd);
    return 0;
}
