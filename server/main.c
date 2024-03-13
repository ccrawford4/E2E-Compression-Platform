#include "main.h"

#define MAX_BUFFER_LEN 1000

void print_out_contents(int sockfd) {
    char* buffer = (char*)malloc(MAX_BUFFER_LEN);

    int bytes_received;
    while ((bytes_received = receive_packets(sockfd, buffer, MAX_BUFFER_LEN)) > 0) {
        printf("%s\n", buffer);
        memset(buffer, 0, MAX_BUFFER_LEN);
    }
    free(buffer);
    if (bytes_received < 0) {
        handle_error(sockfd, "bytes_recieved");
    }
    char* server_msg = "File Received!";
    send_packets(sockfd, server_msg, strlen(server_msg) + 1);
}

int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    const char* server_port = argv[1];

    int sockfd = init_socket(server_port);

    int client_socket = server_listen(sockfd);

    print_out_contents(client_socket);

    close_sockets(sockfd, client_socket);

    return 0;

}
