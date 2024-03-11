#include "../shared/shared.h"
int establish_connection(const char*server_ip, unsigned short server_port);
int receive_bytes(int sockfd, char *buf, int len, int flags);
int send_bytes(int sockfd, char *buf, int len, int flags);
