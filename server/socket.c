#include "main.h"
// socket()
// bind()
// listen()
// accept()

// read()
// write()

int send_packets(int client_socket, char *buffer, int buffer_len) {
    size_t sent_bytes = send(client_socket, buffer, buffer_len, 0);
    
    if (sent_bytes < 0) {
        handle_error(client_socket, "sendto() failed");
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
        handle_error(sockfd, "setsockopt()");
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    if (bind(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        handle_error(sockfd, "bind() cannot bind socket to address");
    } 

}

int server_listen(int sockfd) {
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
    return client_sockfd;
}

int init_socket(const char* server_port) {
    // Create the socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        handle_error(sockfd, "socket()");
    }
    unsigned int port_number = (unsigned int)atoi(server_port);
    // Bind and set the socket options
    bind_socket(sockfd, port_number);
    
    return sockfd;
}

int init_udp_socket(const char* server_port) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        handle_error(sockfd, "udp->socket()");
        exit(EXIT_FAILURE);
    }
    unsigned int port_number = (unsigned int)atoi(server_port);
    // bind and set sockoptions
    bind_socket(sockfd, port_number);
    return sockfd;
}

ssize_t receive_udp_payload(int sockfd, struct sockaddr *src_addr, socklen_t addrlen) {
    char* buffer = (char*)malloc(1000);
    memset(buffer, 0, sizeof(buffer) / sizeof(char));
    size_t len = strlen(buffer);
    ssize_t bytes = recvfrom(sockfd, buffer, len, 0, src_addr, &addrlen);
    return bytes;
}


void clean_exit() {exit(0);};

void close_sockets(int sockfd, int client_socket) {
    close(sockfd);
    close(client_socket);
    signal(SIGTERM, clean_exit);
    signal(SIGINT, clean_exit);
}
