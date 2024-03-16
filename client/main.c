#include "main.h"
#define MAX_BUFFER_LEN 500

// Sends the file contents given a file path
void send_file_contents(int sockfd, char* file_path) {
    char* file_contents = read_file(file_path);
    int len = strlen(file_contents);
    if (send_bytes(sockfd, file_contents, len, 0)) {
        printf("Sending complete.\n");
    } else {
        handle_error(sockfd, "Send()");
    }
    free(file_contents);
}

// Receives messages from the server
void receive_server_msg(int sockfd) {
    char *buffer = (char*)malloc(MAX_BUFFER_LEN);
    memset(buffer, 0, sizeof(buffer));
    int len = strlen(buffer);
    int bytes_recv = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
    if (bytes_recv == -1) {
        handle_error(sockfd, "Recv()");
    }
   printf("Server Message: %s\n", buffer);
    free(buffer);
}



int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_client <file_name.json>\n");
        return EXIT_FAILURE;
    }
    char* file_name = argv[1];
    size_t size = snprintf(NULL, 0, "%s%s", PATH_PREFIX, file_name) + 1;

    char* full_path = malloc(size);
    if (full_path == NULL) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }

    snprintf(full_path, size, "%s%s", PATH_PREFIX, file_name);

    // Get the Server IP and Server TCP_PREPROB port number for preprobing phase
    const char* server_address = get_value(full_path, "server_ip");
    unsigned short server_port = (unsigned short)atoi(get_value(full_path, "TCP_PREPROB_port_number"));
    if (server_port == 0) {
        perror("Error! Invalid TCP_PREPROB_port_number");
        return EXIT_FAILURE;
    }
        
    // Inital TCP Connection (Pre-Probing Phase)
    int tcp_socket = establish_connection(server_address, server_port);
    send_file_contents(tcp_socket, full_path);
    receive_server_msg(tcp_socket);

    // Probing Phase
    int udp_src_port = atoi(get_value(full_path, "UDP_src_port_number"));
    int udp_socket = init_udp_socket(udp_src_port);
    char* udp_payload_string = (char*)get_value(full_path, "UDP_payload_size");
    int len = strlen(udp_payload_string);
    *(udp_payload_string + len - 1) = '\0';         // Remove the 'B' from the payload_size
    
    int udp_payload_size = atoi(udp_payload_string);
    if (udp_payload_size == 0) {
        perror("Error! Invalid UDP_payload_size");
        return EXIT_FAILURE;
    }

    int udp_packet_train_size = atoi(get_value(full_path, "UDP_packet_train_size"));
    if (udp_packet_train_size == 0) {
        perror("ERROR! Invalid UDP_packet_train_size");
        return EXIT_FAILURE;
    }

    int udp_dest_port_number = atoi(get_value(full_path, "UDP_dest_port_number"));
    send_udp_packets(udp_socket, server_address, udp_dest_port_number, udp_payload_size, udp_packet_train_size, true);


    free(full_path);
 

    close(tcp_socket);

}
