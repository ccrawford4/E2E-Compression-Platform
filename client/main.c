#include "main.h"
#define MAX_BUFFER_LEN 500
#define RESULT_FILE "result.txt"

// Sends the file contents given a file path
void send_file_contents(int sockfd, char* file_path) {
    char* file_contents = read_file(file_path);
    int len = strlen(file_contents);
    if (send_bytes(sockfd, file_contents, len, 0)) {
    } else {
        handle_error(sockfd, "Send()");
    }
    free(file_contents);
}

void write_file_contents(char* buffer, int len) {
    FILE* fp = fopen(RESULT_FILE, "w");
    if (fp == NULL) {
        perror("Failed to open the file");
        exit(EXIT_FAILURE);
    }

    size_t bytes_written = fwrite(buffer, 1, len, fp);
    if (bytes_written < len) {
        perror("Failed to write full buffer to file");
    }
    free(buffer);
    fclose(fp);

}
// Receives messages from the server
void receive_server_msg(int sockfd, bool pre_prob) {
    char *buffer = (char*)malloc(MAX_BUFFER_LEN);
    memset(buffer, 0, sizeof(buffer));
    int len = strlen(buffer);
    int bytes_recv = receive_bytes(sockfd, buffer, MAX_BUFFER_LEN, 0);
    if (bytes_recv == -1) {
        handle_error(sockfd, "Recv()");
    }
    if (pre_prob) {
        printf("[server]: %s\n", buffer);
    } else {
        write_file_contents(buffer, len);
        printf("[client]: Received Results From Server!\n");
    }

    free(buffer);
}

// Establishes a TCP connection
void tcp_connection(char* full_path, char* key, const char* server_address, bool pre_prob) {
    unsigned short port = (unsigned short)atoi(get_value(full_path, key));
    if (port == 0) {
        perror("Error! Invalid TCP_PREPROB_port_number");
        exit(EXIT_FAILURE);
    }
    printf("[client]: Establishing TCP Connection\n");
    int sockfd = establish_connection(server_address, port);
    if (pre_prob) {
        printf("[client]: Sending myconfig.json...\n");
        send_file_contents(sockfd, full_path);
        receive_server_msg(sockfd, pre_prob);
    } else {
        printf("[client]: Waiting for result file\n");
        receive_server_msg(sockfd, pre_prob);
    }
  }

void probe(char* full_path, int udp_socket, const char* server_address, int udp_dest_port, int udp_payload_size, int udp_packet_train_size) {
    unsigned int timer = (unsigned int)atoi(get_value(full_path, "measurement_time"));
    if (timer == 0) {
        perror("ERROR! Invalid measurement_time");
        exit(EXIT_FAILURE);
    }
    unsigned int server_wait_time = (unsigned int)atoi(get_value(full_path, "server_wait_time"));
    if (server_wait_time == 0) {
        perror("ERROR! Invalid server wait time");
        exit(EXIT_FAILURE);
    }

    // Send low entropy
    printf("[client]: Sending first round of UDP packets...\n");
    send_udp_packets(udp_socket, server_address, udp_dest_port, udp_payload_size, udp_packet_train_size, true);
    // Wait
    wait(server_wait_time);             // Performs the wait time to make sure all the inital packets got there
    wait(timer);
    // Send high entropy
    printf("[client]: Sending second round of UDP packets...\n");
    send_udp_packets(udp_socket, server_address, udp_dest_port, udp_payload_size, udp_packet_train_size, false);
    wait(timer);
}

// Probing Phase
void probing_phase(char* full_path, const char* server_address) {
    int udp_dest_port = atoi(get_value(full_path, "UDP_dest_port_number"));
    if (udp_dest_port == 0) {
        perror("ERROR! Invalid UDP_src_port_number");
        exit(EXIT_FAILURE);
    }
    int udp_src_port = atoi(get_value(full_path, "UDP_src_port_number"));

    int udp_socket = init_udp_socket(udp_src_port);
    char* udp_payload_string = (char*)get_value(full_path, "UDP_payload_size");
    int len = strlen(udp_payload_string);
    *(udp_payload_string + len - 1) = '\0';         // Remove the 'B' from the payload_size
    
    int udp_payload_size = atoi(udp_payload_string);
    if (udp_payload_size == 0) {
        perror("Error! Invalid UDP_payload_size");
        exit(EXIT_FAILURE);
    }

    int udp_packet_train_size = atoi(get_value(full_path, "UDP_packet_train_size"));
    if (udp_packet_train_size == 0) {
        perror("ERROR! Invalid UDP_packet_train_size");
        exit(EXIT_FAILURE);
    }

    probe(full_path, udp_socket, server_address, udp_dest_port, udp_payload_size, udp_packet_train_size);
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
     
     tcp_connection(full_path, "TCP_PREPROB_port_number", server_addr, true);    // Pre-Probing Phase TCP Connection
     
     probing_phase(full_path, server_addr);// Probing Phase

     tcp_connection(full_path, "TCP_POSTPROB_port_number", server_addr, false);  // Post-Probing Phase TCP Connection
   
    free(full_path);
 
}
