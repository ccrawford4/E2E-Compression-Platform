#include "main.h"

#define MAX_BUFFER_LEN 500
// parse_json()

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

    const char* server_address = get_value(full_path, "server_ip");
    const char* port = get_value(full_path, "UDP_dest_port_number");

    unsigned short server_port = (unsigned short)atoi(port);
    
    // Inital TCP Connection
    int tcp_socket = establish_connection(server_address, server_port);
    send_file_contents(tcp_socket, full_path);
    free(full_path);
    receive_server_msg(tcp_socket);

    int udp_socket = init_udp_socket(server_port);
    // TODO: Implement this
    send_udp_packets(udp_socket, true);
 

    close(tcp_socket);

}
