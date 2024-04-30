#include "../shared/shared.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define RANDOM_FILE "../shared/random_file"

// Creates and binds a socket given a port number and protocol
int init_socket(unsigned short port, int protocol) {
  int sockfd;
  if ((sockfd = socket(AF_INET, protocol, 0)) < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  // Source IP address configuration
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket to the address
  if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    handle_error(sockfd, "bind()");
  }

  // Set the socket to non-blocking
  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
      0) {
    handle_error(sockfd, "setsockopt()");
  }

  return sockfd;
}

// Establishes a TCP connection given the server's IP address and port number
int establish_connection(char *server_ip, unsigned short port) {
  int sockfd = init_socket(port, SOCK_STREAM); // Creates a TCP socket

  // Finds the servers host info given its IP address
  struct hostent *host = gethostbyname(server_ip);
  if (host == NULL) {
    handle_error(sockfd, "Error resolving host");
  }

  // Destination address IP configuration
  struct sockaddr_in sin;
  in_addr_t server_addr = *(in_addr_t *)host->h_addr_list[0];
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = server_addr;
  sin.sin_port = htons(port);

  // Connects the socket to the server address
  if (connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    perror("cannont connect to server");
    exit(EXIT_FAILURE);
  }

  return sockfd;
}

// Sends UDP packets
void send_udp_packets(int sockfd, struct sockaddr_in server_addr,
                      int server_port, int packet_size, int num_packets,
                      bool low_entropy) {
  char *payload = (char *)malloc(packet_size);
  if (payload == NULL) {
    perror("Memory allocation failed\n");
    abort();
  }
  memset(payload, 0, packet_size);

  // Opens the file with the random data for high entropy packets
  FILE *fp = fopen(RANDOM_FILE, "rb");
  if (fp == NULL) {
    free(payload);
    printf("Error Opening File %s\n", RANDOM_FILE);
    exit(EXIT_FAILURE);
  }

  // Send the packets
  for (int i = 0; i < num_packets; i++) {
    // If sending high entropy packets then populate the buffer with the random
    // data
    if (!low_entropy) {
      fseek(fp, 0, SEEK_SET);
      size_t bytes_read = fread(payload, 1, packet_size, fp);
      if (bytes_read < packet_size) {
        fprintf(stderr,
                "Failed to read %d bytes from the file for packet %d.\n",
                packet_size, i);
        break;
      }
    }

    // Set the payload ID
    payload[0] = i & 0xFF;
    payload[1] = (i >> 8) & 0xFF;

    // Send the packet
    ssize_t bytes_sent =
        sendto(sockfd, payload, packet_size, 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_sent < 0) {
      perror("sendto()");
      exit(EXIT_FAILURE);
    }
  }

  fclose(fp);
  free(payload);
}
