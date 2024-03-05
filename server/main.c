#include "main.h"



int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    unsigned short server_port = (unsigned short) atoi(argv[1]);
    if (!server_port) {
        printf("ERROR! %s IS NOT A VALID PORT NUMBER\n", argv[1]);
        return EXIT_FAILURE;
    }

    int sockfd = init_socket(server_port);


  /*  char* file_name = argv[1];
    size_t size = snprintf(NULL, 0, "%s%s", PATH_PREFIX, file_name) + 1;

    char* full_path = malloc(size);
    if (full_path == NULL) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }

    snprintf(full_path, size, "%s%s", PATH_PREFIX, file_name);

    const char* server_address = get_value(full_path, "server_ip");
    const char* port = get_value(full_path, "UDP_dest_port_number");

    free(full_path);

    unsigned short server_port = (unsigned short)atoi(port);
    
    establish_connection(server_address, server_port);*/

}
