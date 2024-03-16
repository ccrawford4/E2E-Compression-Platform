#include "main.h"

#define MAX_BUFFER_LEN 1000
#define FILE_NAME "config.json"

void write_contents_to_file(int sockfd, char* buffer, size_t buffer_size) {
    FILE* fp = fopen(FILE_NAME, "wb");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    size_t bytes_written = fwrite(buffer, 1, buffer_size, fp);
    if (bytes_written < strlen(buffer)) {
          perror("Failed to write full buffer to file");
    }

    free(buffer);
    fclose(fp);
}

void print_out_contents(int sockfd) {
    char* buffer = (char*)malloc(MAX_BUFFER_LEN);

    int bytes_received = receive_packets(sockfd, buffer, MAX_BUFFER_LEN);
    size_t buffer_size = strlen(buffer);
    
    printf("%s\n", buffer);
    write_contents_to_file(sockfd, buffer, buffer_size);

    if (bytes_received < 0) {
        handle_error(sockfd, "bytes_recieved");
    }

    char* server_msg = "File Received!";
    send_packets(sockfd, server_msg, strlen(server_msg) + 1);
}

void recv_udp_packets(int sockfd, unsigned int server_port) {
    char* buffer = (char*)malloc(10000);
    memset(buffer, 0, sizeof(buffer) / sizeof(char));

    struct sockaddr_in src_addr;
    memset(&src_addr, 0, sizeof(src_addr));

    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = INADDR_ANY;
    // TODO: possibly change based on what port it should be
    src_addr.sin_port = htons(server_port);    

    ssize_t bytes = receive_udp_payload(sockfd, (struct sockaddr *)&src_addr, (socklen_t)sizeof(src_addr));

   // TODO: remove hard-coded values
   while (bytes < 6000) {
        bytes += receive_udp_payload(sockfd, (struct sockaddr *)&src_addr, (socklen_t)sizeof(src_addr));
   }

   printf("Received all UDP packets!\n");
}

int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    // TODO: Change this to use pre-probing socket vs post-probing socket
    const char* server_port = argv[1];
    int sockfd = init_socket(server_port);
    int client_socket = server_listen(sockfd);
    
    // Establish TCP Connection
    print_out_contents(client_socket); // For testing and sending confirmation   

    int udp_socket = init_udp_socket(server_port);

    char* buffer = (char*)malloc(10000);
    memset(buffer, 0, sizeof(buffer) / sizeof(char));
    

    recv_udp_packets(udp_socket, server_port);
    
    close_sockets(sockfd, client_socket);

    return 0;

}
