#include "main.h"

// Send TCP packets
int send_packets(int client_socket, char *buffer, int buffer_len) {
    size_t sent_bytes = send(client_socket, buffer, buffer_len, 0);
    
    if (sent_bytes < 0) {
        handle_error(client_socket, "sendto() failed");
    }
    return sent_bytes;
}


// Binds socket
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

// Listen for and accept incoming connections
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

// Creates and configures a socket then returns the socket file descriptor
int init_socket(unsigned int port_number, int type) {
    int sockfd = socket(AF_INET, type, 0);
    if (sockfd == -1) {
        handle_error(sockfd, "socket()");
        return EXIT_FAILURE;
    }
    bind_socket(sockfd, port_number);
    return sockfd;
}

// Receives UDP packets
ssize_t receive_udp_payload(int sockfd, struct sockaddr *src_addr, socklen_t addrlen) {
    char* buffer = (char*)malloc(1000);
    if (buffer == NULL) {
        handle_error(sockfd, "Memory Allocation");
        return EXIT_FAILURE;
    }
    memset(buffer, 0, sizeof(buffer) / sizeof(char));
    size_t len = strlen(buffer);
    ssize_t bytes;
    if ((bytes = recvfrom(sockfd, buffer, len, 0, src_addr, &addrlen)) < 0) {
        handle_error(sockfd, "recvfrom()");
        return EXIT_FAILURE;
    }
    free(buffer);
    return bytes;
}

ssize_t send_udp_packets(int sockfd, struct sockaddr *dest_addr) {
    const char* buf = "All UDP Packets Received!";
    size_t len = strlen(buf);
    ssize_t bytes_sent = sendto(sockfd, buf, len, 0, dest_addr, sizeof(struct sockaddr));
    if (bytes_sent != len) {
        handle_error(sockfd, "sendto()");
    }
    return sockfd;
}

// Exit functions
void clean_exit() {exit(0);};

void close_sockets(int sockfd, int client_socket) {
    close(sockfd);
    close(client_socket);
    signal(SIGTERM, clean_exit);
    signal(SIGINT, clean_exit);
}
