#include "main.h"

#define MAX_BUFFER_LEN 1000
#define R 0.100
#define CONFIG_FILE "config.json"

// Boolean flag set to false
// Will be changed if compression is detected over the network
int compression_flag = 0;

// Set the compression flag
void set_compression_flag(int value) {
    compression_flag = value;
}

// Calculate if compression was detected and return true if it has been
bool calc_results(double time_one, double time_two) {
  double diff = abs(time_one - time_two);
  if (diff > R) {                        
    return true;
  }
  return false;
}

// Receives and saves the config file from Pre-Probing TCP Connection Phase
void recv_config_file(int sockfd) {
  char *buffer = (char *)malloc(MAX_BUFFER_LEN);
  if (buffer == NULL) {
    perror("Memory allocation failure");
    exit(EXIT_FAILURE);
  }

  // Receives the config file from the client
  int bytes_received = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
  size_t buffer_size = strlen(buffer);

  // Writes the bytes received into a new config file in order to store the data for later use
  write_contents_to_file(CONFIG_FILE, buffer, buffer_size);

  if (bytes_received < 0) {
    handle_error(sockfd, "bytes_recieved");
  }
  free(buffer);
}

// Sends the results to the client
void send_results(int sockfd) {
  char* buffer = (char*)malloc(MAX_BUFFER_LEN);
  if (buffer == NULL) {
        perror("Memory allocation failure");
        exit(EXIT_FAILURE);
  }

  // Populate the buffer based on compression results
  if (compression_flag) {
       strcpy(buffer, "Compression Detected!\n");
  } else {
       strcpy(buffer, "No Compression Detected!\n");
  }

  // Send to the results to the client
  ssize_t packets = send(sockfd, buffer, strlen(buffer), 0);

  if (packets < 0) {
    handle_error(sockfd, "send()");
  }

  free(buffer);
}

// Establishes a TCP Connection and based on the phase performs operations
void establish_tcp_connection(unsigned int server_port, bool pre_prob) {
  // TCP socket used for PRE-PROB TCP phase
  int tcp_socket = init_socket(server_port, SOCK_STREAM);               

  // TCP socket used for the POST-PROB TCP phase
  int client_socket = server_listen(tcp_socket);                         

  // During PRE-PROB phase receive the config file - otherwise send the compression results
  if (pre_prob) {
    recv_config_file(client_socket);
  } else {
    send_results(client_socket);
  }
  
  // Close both sockets after use
  close(tcp_socket);
  close(client_socket);
}

// Receives the UDP packets and calculates the stream time
double calc_stream_time(unsigned int server_wait_time,
                        struct sockaddr_in cliaddr, int sockfd) {
  // Buffer used to receive the payload
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

  // Initalize timespec structs
  // start_time = start of UDP stream, end_time = end of UDP stream
  // current_time = current time used for UDP stream timeout
  struct timespec start_time, current_time, end_time;
  clock_gettime(CLOCK_MONOTONIC, &start_time);
  end_time = start_time; 


  while (true) {
    // Get the current time and calculate how many seconds have elapsed
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    double elapsed = (current_time.tv_sec - start_time.tv_sec) +
                     (current_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

    // If timeout has been reached then end the loop
    if (elapsed >= server_wait_time) {
      break;
    }

    // Receive the UDP packet from the client
    n = recvfrom(sockfd, buffer, MAX_BUFFER_LEN - 1, 0,
                 (struct sockaddr *)&cliaddr, &len);
    if (n > 0) {
      end_time = current_time; // Update end_time each time data is received
    }
  }
  free(buffer);

  // After the loop, calculate how long the stream took to arrive to the server
  double total_elapsed = (end_time.tv_sec - start_time.tv_sec) +
                         (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;
    
  return total_elapsed;     // Return the total stream time
}

// Probing Phase -> receive UDP packets from the sender
void probing_phase(unsigned int port, unsigned int server_wait_time,
                   unsigned int client_wait_time) {
  // Client IP address struct
  struct sockaddr_in cliaddr;
  int sockfd = init_socket(port, SOCK_DGRAM);       // Creates a UDP socket

  int n;

  // Receive first UDP stream
  double time_one = calc_stream_time(server_wait_time, cliaddr, sockfd);

  // Wait to avoid packet overlap
  wait(client_wait_time);

  // Receive second UDP stream
  double time_two = calc_stream_time(server_wait_time, cliaddr, sockfd);

  // Calculate results and set the compression flag accordingly
  int compression_flag = calc_results(time_one, time_two) ? 1 : 0;
  set_compression_flag(compression_flag);

}

int main(int argc, char **argv) {
  // Require the user to include the receiving TCP port number for the inital TCP handshake (Pre-prob phase)
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

  // Establish the connection by listening for the client's request to connect
  establish_tcp_connection(server_port,
                           true);

  // Get the necssary values from the JSON that was created in the pre-prob phase
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

  // Send the UDP streams and calculate the results
  probing_phase(udp_port, server_wait_time, client_wait_time); 

  // Establish Post-Prob TCP handshake and send the compression results to the client
  establish_tcp_connection(post_prob_tcp_port,
                           false); 
  return EXIT_SUCCESS;
}
