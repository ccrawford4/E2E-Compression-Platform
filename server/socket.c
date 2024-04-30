#include "main.h"

#define MAX_BUFFER_LEN 1000

// Binds a socket given the file descriptor and the port number
void bind_socket(int sockfd, unsigned short port) {
  int optval = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <
      0) {
    handle_error(sockfd, "setsockopt()");
  }
  
  // Source IP address configuration
  struct sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    handle_error(sockfd, "bind() cannot bind socket to address");
  }
}

// Listen for and accept incoming connections
int server_listen(int sockfd) {
  if (listen(sockfd, 5) < 0) {
    handle_error(sockfd, "listen() error listening");
  }

  // Accept incoming TCP connections
  struct sockaddr_in addr;
  int client_sockfd, addr_len = sizeof(addr);
  client_sockfd = accept(sockfd, (struct sockaddr *)&addr, &addr_len);
  if (client_sockfd < 0) {
    close(client_sockfd);
    handle_error(sockfd, "accept() error accepting connections");
  }
  return client_sockfd;
}

// Creates and binds a socket given a port number and protocol
int init_socket(unsigned int port_number, int proto) {
  int sockfd = socket(AF_INET, proto, 0);
  if (sockfd == -1) {
    handle_error(sockfd, "socket()");
  }
  bind_socket(sockfd, port_number);
  return sockfd;
}

// Receives UDP packets
ssize_t receive_udp_payload(int sockfd, struct sockaddr *src_addr, 
                            socklen_t addrlen) {  
  // Create recv buffer
  char *buffer = (char *)malloc(MAX_BUFFER_LEN);
  if (buffer == NULL) {
    handle_error(sockfd, "Memory Allocation");
    return EXIT_FAILURE;
  }
  memset(buffer, 0, sizeof(buffer) / sizeof(char));
  size_t len = strlen(buffer);
  ssize_t bytes;

  // Receive UDP packets from the client
  if ((bytes = recvfrom(sockfd, buffer, len, 0, src_addr, &addrlen)) < 0) {
    handle_error(sockfd, "recvfrom()");
    return EXIT_FAILURE;
  }
  free(buffer);
  return bytes;
}
