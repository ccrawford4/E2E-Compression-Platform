#include "../shared/shared.h"

void handle_error(int sockfd, char* error_msg);
int init_socket(const char* socket_port);
int send_packets(int sockfd, char* buffer, int buffer_len);
int receive_packets(int sockfd, char *buffer, int buffer_len);
int server_listen(int sockfd);
void clean_exit();
void close_sockets();
