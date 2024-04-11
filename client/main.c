#include "main.h"

#define MAX_BUFFER_LEN 500
#define RESULT_FILE "result.txt"

#ifndef DEBUG
#define DEBUG 0
#endif

// Receives congestion results from the server
void receive_results(int sockfd) {
  char buffer;
  ssize_t bytes_recv = recv(sockfd, &buffer, sizeof(buffer), 0);
  if (bytes_recv > 0) {
    int flag = buffer;
    if (flag) {
        printf("Compression Detected!\n");
    } else {
        printf("No Compression Detected!\n");
    }
  }

#if DEBUG
  printf("[client]: Received Results From Server!\n");
#endif

}

// Establishes a TCP connection
void tcp_connection(char *full_path, unsigned int port,
                    const char *server_address, bool pre_prob) {
#if DEBUG
  printf("[client]: Establishing TCP Connection\n");
#endif
  int sockfd = establish_connection(server_address, port);
  if (pre_prob) {
#if DEBUG
    printf("[client]: Sending myconfig.json...\n");
#endif
    send_file_contents(sockfd, full_path);
  } else {
#if DEBUG
    printf("[client]: Waiting for result file\n");
#endif
    receive_results(sockfd);
  }
  close(sockfd);
}

// Probing Phase
void probing_phase(const char *server_ip, unsigned int server_wait_time,
                   unsigned int measurement_time, int dst_port, int src_port,
                   int payload_size, int train_size) {
  int sockfd = init_socket(src_port, SOCK_DGRAM); // Creates a UDP socket
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(dst_port);

  // Convert the server_ip to binary form
  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    handle_error(sockfd, "inet_pton()");
  }

#if DEBUG
  printf("[client]: Sending first round of UDP packets...\n");
#endif

#if DEBUG
  printf("[client]: Sending first round of UDP packets...\n");
#endif

  // Send low entropy
  send_udp_packets(sockfd, server_addr, dst_port, payload_size, train_size,
                   true);

  // Wait
  wait(server_wait_time);
  wait(measurement_time);

#if DEBUG
  printf("[client]: Sending second round of UDP packets...\n");
#endif

  // Send high entropy
  send_udp_packets(sockfd, server_addr, dst_port, payload_size, train_size,
                   false);
  wait(measurement_time);

  close(sockfd);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: \n");
    printf("./compdetect_client <file_name.json>\n");
    return EXIT_FAILURE;
  }

  char *file_name = argv[1];
  size_t size = snprintf(NULL, 0, "%s%s", PATH_PREFIX, file_name) + 1;
  char *full_path = (char *)malloc(size);
  if (full_path == NULL) {
    perror("Failed to allocate memory");
    return EXIT_FAILURE;
  }
  snprintf(full_path, size, "%s%s", PATH_PREFIX, file_name);

  const char *server_ip = get_value(full_path, "server_ip");
  if (!strcmp(server_ip, "ERROR")) {
    printf("ERROR! You must enter a server IP address in the %s file\n",
           argv[1]);
    return EXIT_FAILURE;
  }

  // Get necessary values from the JSON
  unsigned int tcp_preprob_port =
      (unsigned int)atoi(get_value(full_path, "TCP_PREPROB_port_number"));
  unsigned int tcp_postprob_port =
      (unsigned int)atoi(get_value(full_path, "TCP_POSTPROB_port_number"));
  unsigned int server_wait_time =
      (unsigned int)atoi(get_value(full_path, "server_wait_time"));
  unsigned int measurement_time =
      (unsigned int)atoi(get_value(full_path, "measurement_time"));
  unsigned int dst_port =
      (unsigned int)atoi(get_value(full_path, "UDP_dest_port_number"));
  unsigned int src_port =
      (unsigned int)atoi(get_value(full_path, "UDP_src_port_number"));
  char *udp_payload_string = (char *)get_value(full_path, "UDP_payload_size");
  int len = strlen(udp_payload_string);
  *(udp_payload_string + len - 1) =
      '\0'; // Remove the 'B' from the payload_size
  unsigned int payload_size = (unsigned int)atoi(udp_payload_string);
  unsigned int train_size =
      (unsigned int)atoi(get_value(full_path, "UDP_packet_train_size"));

  // Handle errors if the JSON is not properly initalized
  handle_key_error(tcp_preprob_port, "TCP_PREPROB_port_number", full_path);
  handle_key_error(tcp_postprob_port, "TCP_POSTPROB_port_number", full_path);
  handle_key_error(server_wait_time, "server_wait_time", full_path);
  handle_key_error(measurement_time, "measurement_time", full_path);
  handle_key_error(dst_port, "UDP_dest_port_number", full_path);
  handle_key_error(src_port, "UDP_src_port_number", full_path);
  handle_key_error(payload_size, "UDP_payload_size", full_path);
  handle_key_error(train_size, "UDP_packet_train_size", full_path);

  // Pre-Probing Phase TCP Connection
  tcp_connection(full_path, tcp_preprob_port, server_ip, true);

  // Probing Phase
  probing_phase(server_ip, server_wait_time, measurement_time, dst_port,
                src_port, payload_size, train_size);

  // Post-Probing Phase TCP Connection
  tcp_connection(full_path, tcp_postprob_port, server_ip, false);

  free(full_path);

  return EXIT_SUCCESS;
}
