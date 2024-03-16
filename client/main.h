#include "../shared/shared.h"
#include <stdbool.h>
int establish_connection(const char*server_ip, unsigned short server_port);
int receive_bytes(int sockfd, char *buf, int len, int flags);
int send_bytes(int sockfd, char *buf, int len, int flags);
int init_udp_socket(unsigned int server_port);
void send_udp_packets(int udp_socket, const char* server_ip, unsigned short server_port, int packet_size, int num_packets, bool low_entropy);

