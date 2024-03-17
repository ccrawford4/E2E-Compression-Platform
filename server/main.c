#include "main.h"

#define MAX_BUFFER_LEN 1000
#define FILE_NAME "config.json"

// Write the JSON contents to the config.json file
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

// Prints out JSON contents from Pre-Probing TCP Connection Phase
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

// Receives UDP packets given a port number
void recv_udp_packets(int sockfd, unsigned int port_number) {
    char* buffer = (char*)malloc(10000);
    memset(buffer, 0, sizeof(buffer) / sizeof(char));

    struct sockaddr_in src_addr;
    memset(&src_addr, 0, sizeof(src_addr));

    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = INADDR_ANY;
    src_addr.sin_port = htons(port_number);    

    ssize_t bytes = receive_udp_payload(sockfd, (struct sockaddr *)&src_addr, (socklen_t)sizeof(src_addr));
   if (bytes != -1) {
      printf("Received all UDP packets!\n");
   }
}

// Establishes a TCP Connection
void establish_tcp_connection(unsigned int server_port) {
    unsigned int server_port = (unsigned int) atoi(argv[1]);
    if (server_port == 0) {
        printf("ERROR! %s Is Not A Valid Port Number\n", argv[1]);
        return EXIT_FAILURE;
    }
    int tcp_socket = init_socket(server_port, SOCK_STREAM);
    int client_socket = server_listen(tcp_socket);
    
    // Establish TCP Connection
    print_out_contents(client_socket); // For testing and sending confirmation   
}

// Probing Phase -> receive UDP packets from the sender
void probing_phase() { 
    unsigned int udp_port = (unsigned int)atoi(get_value(FILE_NAME, "UDP_dest_port_number"));
    if (udp_port == 0) {
        handle_error(udp_port, "Invalid UDP_dest_port_number");
        return EXIT_FAILURE;
    }

    int udp_socket = init_socket(udp_port, SOCK_DGRAM);  
    recv_udp_packets(udp_socket, udp_port);

}

int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    unsigned int server_port = (unsigned int) atoi(argv[1]);
    if (server_port == 0) {
        printf("ERROR! %s Is Not A Valid Port Number\n", argv[1]);
        return EXIT_FAILURE;
    }
    
    establish_tcp_connection(server_port);         // Pre-Probing TCP Phase Connection
    probing_phase();                               // Probing Phase
        
    return EXIT_SUCCESS;
}
