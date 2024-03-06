#include "main.h"



int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    const char* server_port = argv[1];

    int sockfd = init_socket(server_port);
 

}
