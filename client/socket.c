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

void establish_connection(char* server_ip, unsigned short server_port) {
   int sockfd = socket(AF_INET, SOCK_DGRAM, SOCK_DGRAM);

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

}
