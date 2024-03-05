#include "main.h"
// socket()
// bind()
// listen()
// accept()

// read()
// write()

void bind_socket(int sockfd, unsigned short server_port) {
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(sockfd));

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("cannot bind socket to address");
        abort();
    } 

}

void server_listen(int sockfd) {
    if (listen(sockfd, 5) < 0) {
       perror("error listening");
       abort();
    }

    struct sockaddr_in addr;
    int client_sockfd, addr_len = sizeof(addr);
    client_sockfd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);
    if (client_sockfd < 0) {
        perror("error accepting connections");
        abort();
    }
}

int init_socket(unsigned short server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, SOCK_DGRAM);
    bind_socket(sockfd, server_port);
    return sockfd;
}
