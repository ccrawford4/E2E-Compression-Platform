#include "main.h"

#define MAX_BUFFER 1000

void print_out_contents(int sockfd) {
    char* buffer = (char*)malloc(MAX_BUFFER);
    int buffer_len = sizeof(buffer);

    while (receive_packets(sockfd, buffer, buffer_len) != EOF) {
        printf("%s\n", buffer);
    }
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

    print_out_contents(sockfd);

    close_sockets(sockfd, client_socket);

    return 0;

}
