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

struct addrinfo init_hints() {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = IPPROTO_TCP;
    return hints;
}

int init_socket(const char* server_port) {
    struct addrinfo hints = init_hints();

    struct addrinfo *results;

    int e = getaddrinfo(NULL, server_port, &hints, &results);
    if (e != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(e));
        exit(EXIT_FAILURE);
    }
    
    int sockfd = -1;
    for (struct addrinfo *r = results; r != NULL; r = r->ai_next) {
        sockfd = socket(r->ai_family, r->ai_socktype, r->ai_protocol);
        if (sockfd == -1) {
            continue;
        }
        int reuse_addr = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1) 
        {
            handle_error(sockfd, "setsockopt()");
        }
        if (bind(sockfd, r->ai_addr, r->ai_addrlen) == -1) {
            close(sockfd);
            continue;
        }
        break;
    }

    return sockfd;
}

void clean_exit() {exit(0);};

void close_sockets(int sockfd, int client_socket) {
    close(sockfd);
    close(client_socket);
    signal(SIGTERM, clean_exit);
    signal(SIGINT, clean_exit);
}
