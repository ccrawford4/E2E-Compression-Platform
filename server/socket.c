#include "main.h"
// socket()
// bind()
// listen()
// accept()

// read()
// write()


void handle_error(int sockfd, char* error_msg) {
    perror(error_msg);
    close(sockfd);
    abort();
}


int send_packets(int sockfd, char *buffer, int buffer_len) {
    size_t sent_bytes = send(sockfd, buffer, buffer_len, 0);
    
    if (sent_bytes < 0) {
        handle_error(sockfd, "sendto() failed");
    }
    return sent_bytes;
}

int receive_packets(int sockfd, char *buffer, int buffer_len) {
    int num_recieved = recv(sockfd, buffer, buffer_len, 0);
    
    if (num_recieved < 0) {
        handle_error(sockfd, "recvfrom() failed");
    } else if (num_recieved == 0) {
        /* sender has closed connection */
        return EOF;
    } else {
        return num_recieved; /* might not be full record! */
    }

} 

void bind_socket(int sockfd, unsigned short server_port) {
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        handle_error(sockfd, "setsockopt() couldn't reuse address");
    }
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        handle_error(sockfd, "bind() cannot bind socket to address");
    } 

}

void server_listen(int sockfd) {
    if (listen(sockfd, 5) < 0) {
       handle_error(sockfd, "listen() error listening");
    }

    struct sockaddr_in addr;
    int client_sockfd, addr_len = sizeof(addr);
    client_sockfd = accept(sockfd, (struct sockaddr *) &addr, &addr_len);
    if (client_sockfd < 0) {
        close(client_sockfd);
        handle_error(sockfd, "accept() error accepting connections");
    }
}

int init_socket(unsigned short server_port) {
    int sockfd = 1;
    if (sockfd = socket(AF_INET, SOCK_DGRAM, 0) < 0) {
        handle_error(sockfd, "socket() could not create UDP socket");
    }

    bind_socket(sockfd, server_port);
    return sockfd;
}

void clean_exit() {exit(0);};

void close_sockets() {
    signal(SIGTERM, clean_exit);
    signal(SIGINT, clean_exit);
}
