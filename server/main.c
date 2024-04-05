#include "main.h"
#include <math.h>

#define MAX_BUFFER_LEN 1000
#define MAX_LINE 1000
#define R 0.100
#define CONFIG_FILE "config.json"
#define RESULT_FILE "result.txt"


bool calc_results(double time_one, double time_two) {
    double diff = abs(time_one - time_two);
    if (diff > R) {
        return true;
    }
    return false;
}

void save_results(double time_one, double time_two) {
   double diff = abs(time_one - time_two);
   char* buffer = (char*)malloc(MAX_BUFFER_LEN);
   if (buffer == NULL) {
        perror("Memory allocation failure");
        exit(EXIT_FAILURE);
   }
   
   bool found_compression = calc_results(time_one, time_two);
   int w;
   if (found_compression) {
        w = snprintf(buffer, MAX_BUFFER_LEN, "Compression Detected!\nTime One: %f\tTime Two: %f\n", 
        time_one, time_two);
   } else {
        w = snprintf(buffer, MAX_BUFFER_LEN, "No Compression Detected!\nTime one: %f\tTime Two: %f\n",
        time_one, time_two);
   }

   if (w > MAX_BUFFER_LEN) {
       perror("Buffer length exceeded!\n");
       exit(EXIT_FAILURE);
   }
   
   size_t n = strlen(buffer);
   write_contents_to_file(RESULT_FILE, buffer, n);
}

// Prints out JSON contents from Pre-Probing TCP Connection Phase
void recv_config_file(int sockfd) {
    char* buffer = (char*)malloc(MAX_BUFFER_LEN);

    int bytes_received = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
    size_t buffer_size = strlen(buffer);
    
    write_contents_to_file(CONFIG_FILE, buffer, buffer_size);

    if (bytes_received < 0) {
        handle_error(sockfd, "bytes_recieved");
    }

    char* server_msg = "Config File Received!";
    send_bytes(sockfd, server_msg, strlen(server_msg) + 1, 0);
}

void send_results(int sockfd) {
    char buffer[MAX_BUFFER_LEN]; 
    int n = strlen(buffer);

    // read the file contents into the buffer
    FILE *stream = fopen(RESULT_FILE, "r");
    if (stream == NULL) {
        perror("Failed to open the file");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int count = fread(&buffer, sizeof(char), MAX_BUFFER_LEN, stream);
    fclose(stream);
    int packets = send_bytes(sockfd, buffer, n, 0);
    if (packets != n) {
        perror("ERROR! Not all the packets were received");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

// Establishes a TCP Connection
void establish_tcp_connection(unsigned int server_port, bool pre_prob) {
    int tcp_socket = init_socket(server_port, SOCK_STREAM);
    int client_socket = server_listen(tcp_socket);    
    if (pre_prob) {
        recv_config_file(client_socket);
    } else {
       send_results(client_socket); 
    }    
}

double calc_stream_time(unsigned int server_wait_time, struct sockaddr_in cliaddr, int sockfd) {
    char* buffer = (char*)malloc(1000); // Allocate buffer correctly
    socklen_t len = sizeof(cliaddr);
    int n;

    // Set a timeout for recvfrom
    struct timeval timeout;
    timeout.tv_sec = server_wait_time; // Set the timeout to server_wait_time seconds
    timeout.tv_usec = 0; // No microseconds
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt failed");
    }

    struct timespec start_time, current_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    end_time = start_time; // Initialize end_time to start_time

    while (true) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double elapsed = (current_time.tv_sec - start_time.tv_sec) + 
                         (current_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

        if (elapsed >= server_wait_time) {
            break;
        }

        n = recvfrom(sockfd, buffer, 1000 - 1, 0, (struct sockaddr *)&cliaddr, &len); // Use correct buffer size
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
void probing_phase() { 
    unsigned int udp_port = (unsigned int)atoi(get_value(CONFIG_FILE, "UDP_dest_port_number"));
    if (udp_port == 0) {
        handle_error(udp_port, "Invalid UDP_dest_port_number");
        exit(EXIT_FAILURE);
    }

    int sockfd;
    char* buffer = (char*)malloc(1000);
    if (buffer == NULL) {
        perror("Memory Allocation Failed");
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in serveraddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(udp_port);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    int n;
    unsigned int server_wait_time = (unsigned int)atoi(get_value(CONFIG_FILE, "server_wait_time"));
    unsigned int client_wait_time = (unsigned int)atoi(get_value(CONFIG_FILE, "measurement_time"));
    double time_one = calc_stream_time(server_wait_time, cliaddr, sockfd);
    wait(client_wait_time);
    double time_two = calc_stream_time(server_wait_time, cliaddr, sockfd);

    save_results(time_one, time_two);

}

int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_server <TCP port number>\n");
        return EXIT_FAILURE;
    }

    unsigned int server_port = (unsigned int) atoi(argv[1]);
    if (server_port == 0) {
        printf("ERROR! %s Is Not A Valid Port Number\n", argv[1]);
        return EXIT_FAILURE;
    }

    
    time_t rawtime;
    struct tm * timeinfo;

    establish_tcp_connection(server_port, true);         // Pre-Probing TCP Phase Connection
    
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    printf("Current time before probing phase: %s\n", asctime (timeinfo));
    probing_phase();                               // Probing Phase

    unsigned int post_prob_tcp_port = (unsigned int) atoi(get_value(CONFIG_FILE, "TCP_POSTPROB_port_number"));
    if (post_prob_tcp_port == 0) {
        printf("ERROR! %d Is Not A Valid Port Number\n", post_prob_tcp_port);
        return EXIT_FAILURE;
    }
    
    establish_tcp_connection(post_prob_tcp_port, false);
        
    return EXIT_SUCCESS;
}
