#include "main.h"
#define MAX_BUFFER_LEN 500
#define RESULT_FILE "result.txt"

// Receives messages from the server
void receive_server_msg(int sockfd, bool pre_prob) {
    char *buffer = (char*)malloc(MAX_BUFFER_LEN);
    if (buffer == NULL) {
        perror("Error Allocating Memory");
        exit(EXIT_FAILURE);
    }
    memset(buffer, 0, sizeof(buffer));
    int bytes_recv = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
    if (bytes_recv == -1) {
        handle_error(sockfd, "Recv()");
    }
    if (pre_prob) {
       printf("[server]: %s\n", buffer);
    } else {
        int len = strlen(buffer);
        write_contents_to_file(RESULT_FILE, buffer, len);
        printf("[client]: Received Results From Server!\n");
    }

    free(buffer);
}

// Establishes a TCP connection
void tcp_connection(char* full_path, unsigned int port, const char* server_address, bool pre_prob) {
    printf("[client]: Establishing TCP Connection\n");
    int sockfd = establish_connection(server_address, port);
    if (pre_prob) {
        printf("[client]: Sending myconfig.json...\n");
        send_file_contents(sockfd, full_path);
        receive_server_msg(sockfd, pre_prob);
        wait(1);
    } else {
        printf("[client]: Waiting for result file\n");
        receive_server_msg(sockfd, pre_prob);
    }
    close(sockfd);
 }

// Probing Phase
void probing_phase(const char* server_addr, unsigned int server_wait_time, unsigned int measurement_time,
int dst_port, int src_port, int payload_size, int train_size) {
    int sockfd = init_udp_socket(src_port);

    // Send low entropy
    printf("[client]: Sending first round of UDP packets...\n");
    send_udp_packets(sockfd, server_addr, dst_port, payload_size, train_size, true);

    // Wait
    wait(server_wait_time);
    wait(measurement_time);

    // Send high entropy
    printf("[client]: Sending second round of UDP packets...\n");
    send_udp_packets(sockfd, server_addr, dst_port, payload_size, train_size, false);
    wait(measurement_time);

    close(sockfd);
}

int main(int argc, char**argv) {
    if (argc != 2) {
        printf("usage: \n");
        printf("./compdetect_client <file_name.json>\n");
        return EXIT_FAILURE;
    }

    char* file_name = argv[1];
    size_t size = snprintf(NULL, 0, "%s%s", PATH_PREFIX, file_name) + 1;
    char* full_path = malloc(size);

    if (full_path == NULL) {
        perror("Failed to allocate memory");
        return EXIT_FAILURE;
    }
     snprintf(full_path, size, "%s%s", PATH_PREFIX, file_name);

     const char* server_addr = get_value(full_path, "server_ip");
     if (!strcmp(server_addr, "ERROR")) {
        printf("ERROR! You must enter a server IP address in the %s file\n", argv[1]);
        return EXIT_FAILURE;
     }

     unsigned int tcp_preprob_port = (unsigned int)atoi(get_value(full_path, "TCP_PREPROB_port_number"));
     handle_key_error(tcp_preprob_port, "TCP_PREPROB_port_number", full_path);
     unsigned int tcp_postprob_port = (unsigned int)atoi(get_value(full_path, "TCP_POSTPROB_port_number"));
     handle_key_error(tcp_postprob_port, "TCP_POSTPROB_port_number", full_path); 
     unsigned int server_wait_time = (unsigned int)atoi(get_value(full_path, "server_wait_time"));
     handle_key_error(server_wait_time, "server_wait_time", full_path);
     unsigned int measurement_time = (unsigned int)atoi(get_value(full_path, "measurement_time"));
     handle_key_error(measurement_time, "measurement_time", full_path); 
     unsigned int dst_port = (unsigned int)atoi(get_value(full_path, "UDP_dest_port_number"));
     handle_key_error(dst_port, "UDP_dest_port_number", full_path);
     unsigned int src_port = (unsigned int)atoi(get_value(full_path, "UDP_src_port_number"));
     handle_key_error(src_port, "UDP_src_port_number", full_path);
     char* udp_payload_string = (char*)get_value(full_path, "UDP_payload_size");
     int len = strlen(udp_payload_string);
     *(udp_payload_string + len - 1) = '\0';                                  // Remove the 'B' from the payload_size    
     unsigned int payload_size = (unsigned int)atoi(udp_payload_string);
     handle_key_error(payload_size, "UDP_payload_size", full_path);
     unsigned int train_size = (unsigned int)atoi(get_value(full_path, "UDP_packet_train_size"));
     handle_key_error(train_size, "UDP_packet_train_size", full_path);

     tcp_connection(full_path, tcp_preprob_port, server_addr, true);         // Pre-Probing Phase TCP Connection
     
     probing_phase(server_addr, server_wait_time, measurement_time, dst_port, src_port, payload_size, train_size);    // Probing Phase
    
     tcp_connection(full_path, tcp_postprob_port, server_addr, false);        // Post-Probing Phase TCP Connection
   
     free(full_path);

    return EXIT_SUCCESS;
 
}
