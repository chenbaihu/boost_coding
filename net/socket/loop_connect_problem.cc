/* tcploop.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int port = 0;
    int sckfd = 0;
    int opt = 1;
    struct sockaddr_in *remote;
    char *ip = "127.0.0.1";
    int rc;
    long n = 0;
    if(argc != 2){
        printf("Usage: %s <listen port>\n", argv[0]);
        exit(1);
    }
    port = atoi(argv[1]);

    /* Set to Localhost:Port */
    remote = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in *));
    remote->sin_family = AF_INET;
    assert(inet_pton(AF_INET, ip, (void *)(&(remote->sin_addr.s_addr))) > 0);
    remote->sin_port = htons(port);

    /* create socket */
    assert((sckfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0);
    assert(setsockopt(sckfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0);
    printf("Trying to connect..."); fflush(stdout);
    while(1) {
        n++;
        rc = connect(sckfd, (struct sockaddr *)remote, sizeof(struct sockaddr)); 
        if(rc < 0){
            if(n % 1000 == 0) { printf("."); fflush(stdout); }
            continue;
        }
        else {
            printf("Connected after %ld tries\n", n);
            break;
        }
    }

    /* Wait for Enter */
    printf("Press Enter to Continue...\n");
    getchar();
    close(sckfd);
    return 0;
}




