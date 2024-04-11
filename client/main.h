#include "../shared/shared.h"

int establish_connection(const char*server_ip, unsigned short server_port);
int receive_bytes(int sockfd, char *buf, int len, int flags);
int send_bytes(int sockfd, char *buf, int len, int flags);
int init_socket(unsigned int server_port, int type);
void send_udp_packets(int sockfd, struct sockaddr_in, unsigned short server_port, int packet_size, int num_packets, bool low_entropy);

