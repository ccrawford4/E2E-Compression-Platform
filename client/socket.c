#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Create a socket
// Determine server address & port number (from config file)
// Connect to server
// Write/Read data to connected socket

// socket()
// connect()
// write()
// read()

// Client establishes connection


int init_udp_socket(unsigned short server_port) {
    struct sockaddr_in server_addr;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create UDP socket");
        abort();
    }
    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    return sockfd;
}


int establish_connection(char* server_ip, unsigned short server_port) {
  printf("Server IP: %s\n", server_ip);
  printf("Server Port: %d\n", server_port);
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);

   struct sockaddr_in sin;
   struct hostent *host = gethostbyname(server_ip);
   in_addr_t server_addr = *(in_addr_t *) host->h_addr_list[0];

   memset(&sin, 0, sizeof(sin));  

   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = server_addr;
   sin.sin_port = htons(server_port);

   if (connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("cannont connect to server");
        abort();
   }
   return sockfd;
}

int receive_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_recieved = recv(sockfd, buf, len, flags);
    if (bytes_recieved == -1) {
        perror("error recieving content");
        abort();
    }
    return bytes_recieved;
 }

int send_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_sent = send(sockfd, buf, len, flags);
    if (bytes_sent == -1) {
        perror("error sending content");
        abort();
    }
    return bytes_sent;
}

// TODO: Implementation of this
void send_low_entropy();


