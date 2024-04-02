#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define RANDOM_FILE "../shared/random_file"

// Creates and returns a valid UDP socket
int init_udp_socket(unsigned short src_port) {
    struct sockaddr_in client_addr;
    int sockfd;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Could not create UDP socket");
        abort();
    }
    memset(&client_addr, 0, sizeof(client_addr));

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(src_port);
    client_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("bind()");
        abort();
    }

    return sockfd;
}

// Establishes a TCP connection given the server's IP address and port number
int establish_connection(char* server_ip, unsigned short port) {
   int sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
     perror("socket()");
     exit(EXIT_FAILURE);
   }

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

   struct sockaddr_in sin;
   struct hostent *host = gethostbyname(server_ip);
   if (host == NULL) {
        perror("Error resolving host");
        exit(EXIT_FAILURE);
   }

   in_addr_t server_addr = *(in_addr_t *) host->h_addr_list[0];
   memset(&sin, 0, sizeof(sin));  
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = server_addr;
   sin.sin_port = htons(port);

   if (connect(sockfd, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
        perror("cannont connect to server");
        exit(EXIT_FAILURE);
   }

   return sockfd;
}

// Receives bytes from the server (TCP)
int receive_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_recieved = recv(sockfd, buf, len, flags);
    if (bytes_recieved == -1) {
        perror("error recieving content");
        abort();
    }
    return bytes_recieved;
 }

// Sends bytes to the server (TCP)
int send_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_sent = send(sockfd, buf, len, flags);
    if (bytes_sent == -1) {
        perror("error sending content");
        abort();
    }
    return bytes_sent;
}

// Sends UDP packets
void send_udp_packets(int sockfd, const char* server_ip, int server_port, int packet_size, int num_packets, bool low_entropy) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    
    // Converts the IP address from text to binary form
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton()\n");
        abort();
    }

    char *packet = (char*)malloc(packet_size);
    memset(packet, 0, packet_size);
    if (packet == NULL) {
        perror("Memory allocation failed\n");
        abort();
    }
    
    FILE *fp = fopen(RANDOM_FILE, "rb");
    if (fp == NULL) {
        fclose(fp);
        printf("Error Opening File %s\n", RANDOM_FILE);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_packets; i++) {
        ssize_t bytes_sent;
        if (!low_entropy) {
            fread(packet, sizeof(packet), packet_size, fp);
        }
        bytes_sent = sendto(sockfd, packet, packet_size, 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sent < 0) {
            perror("sendto()");
            exit(EXIT_FAILURE);
        }
    }

    free(packet);
}


