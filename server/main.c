#include "main.h"

#define MAX_BUFFER_LEN 1000
#define R 0.100
#define CONFIG_FILE "config.json"
#define RESULT_FILE "result.txt"

#ifndef DEBUG
#define DEBUG 0
#endif

int compression_flag = 0;

void set_compression_flag(int value) {
    compression_flag = value;
}

// Calculate if Compression was detected
bool calc_results(double time_one, double time_two) {
  double diff = abs(time_one - time_two);
  if (diff > R) {
    return true;
  }
  return false;
}

// Receives and saves the JSON contents from Pre-Probing TCP Connection Phase
void recv_config_file(int sockfd) {
  char *buffer = (char *)malloc(MAX_BUFFER_LEN);
  if (buffer == NULL) {
    perror("Memory allocation failure");
    exit(EXIT_FAILURE);
  }

  int bytes_received = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
  size_t buffer_size = strlen(buffer);

  write_contents_to_file(CONFIG_FILE, buffer, buffer_size);

  if (bytes_received < 0) {
    handle_error(sockfd, "bytes_recieved");
  }
}

// Sends the results to the client
void send_results(int sockfd) {
  char buffer = compression_flag ? 1 : 0;          // 1 for true and 0 for false
  int n = sizeof(buffer);
  ssize_t packets = send(sockfd, &buffer, sizeof(buffer), 0);

  if (packets != n) {
    perror("ERROR! Not all the packets were received");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

}

// Establishes a TCP Connection and based on the phase performs operations
void establish_tcp_connection(unsigned int server_port, bool pre_prob) {
  int tcp_socket = init_socket(server_port, SOCK_STREAM);
  int client_socket = server_listen(tcp_socket);

  if (pre_prob) {
    recv_config_file(client_socket);
  } else {
    send_results(client_socket);
  }
  
  close(tcp_socket);
  close(client_socket);
}

double calc_stream_time(unsigned int server_wait_time,
                        struct sockaddr_in cliaddr, int sockfd) {
  char *buffer = (char *)malloc(MAX_BUFFER_LEN); // Allocate buffer correctly
  if (buffer == NULL) {
    handle_error(sockfd, "Memory allocation failure");
  }

  socklen_t len = sizeof(cliaddr);
  int n;

  // Set a timeout for recvfrom
  struct timeval timeout;
  timeout.tv_sec =
      server_wait_time; // Set the timeout to server_wait_time seconds
  timeout.tv_usec = 0;  // No microseconds
  if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) <
      0) {
    perror("setsockopt failed");
  }

  struct timespec start_time, current_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  end_time = start_time; // Initialize end_time to start_time

#if DEBUG
  print_time(start_time);
#endif

  // Calculates the time it takes for the packet train to be received
  while (true) {
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    double elapsed = (current_time.tv_sec - start_time.tv_sec) +
                     (current_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    if (elapsed >= server_wait_time) {
      break;
    }

    n = recvfrom(sockfd, buffer, MAX_BUFFER_LEN - 1, 0,
                 (struct sockaddr *)&cliaddr, &len);
    if (n > 0) {
      end_time = current_time; // Update end_time each time data is received
    }
  }

  double total_elapsed = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

  free(buffer); // Free allocated memory

  return total_elapsed;
}

// Probing Phase -> receive UDP packets from the sender
void probing_phase(unsigned int port, unsigned int server_wait_time,
                   unsigned int client_wait_time) {
  struct sockaddr_in cliaddr;
  int sockfd = init_socket(port, SOCK_DGRAM);

  int n;

  double time_one = calc_stream_time(server_wait_time, cliaddr, sockfd);
  wait(client_wait_time);
  double time_two = calc_stream_time(server_wait_time, cliaddr, sockfd);

  int compression_flag = calc_results(time_one, time_two) ? 1 : 0;
  set_compression_flag(compression_flag);

}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: \n");
    printf("./compdetect_server <TCP port number>\n");
    return EXIT_FAILURE;
  }

  unsigned int server_port = (unsigned int)atoi(argv[1]);
  if (server_port == 0) {
    printf("ERROR! %s Is Not A Valid Port Number\n", argv[1]);
    return EXIT_FAILURE;
  }
  establish_tcp_connection(server_port,
                           true); // Pre-Probing TCP Phase Connection

  // Get the values from the JSON
  unsigned int udp_port =
      (unsigned int)atoi(get_value(CONFIG_FILE, "UDP_dest_port_number"));
  unsigned int server_wait_time =
      (unsigned int)atoi(get_value(CONFIG_FILE, "server_wait_time"));
  unsigned int client_wait_time =
      (unsigned int)atoi(get_value(CONFIG_FILE, "measurement_time"));
  unsigned int post_prob_tcp_port =
      (unsigned int)atoi(get_value(CONFIG_FILE, "TCP_POSTPROB_port_number"));

  // Handle errors if the values in the JSON are improperly configured
  handle_key_error(udp_port, "UDP_dest_port_number", CONFIG_FILE);
  handle_key_error(server_wait_time, "server_wait_time", CONFIG_FILE);
  handle_key_error(client_wait_time, "measurement_time", CONFIG_FILE);
  handle_key_error(post_prob_tcp_port, "TCP_POSTPROB_port_number", CONFIG_FILE);

  probing_phase(udp_port, server_wait_time, client_wait_time); // Probing Phase

  establish_tcp_connection(post_prob_tcp_port,
                           false); // Post-Prob TCP Phase connection

  return EXIT_SUCCESS;
}
