#include "shared.h"
#define MAX_KEY_LEN 100


void handle_error(int sockfd, char* error_msg) {
    perror(error_msg);
    close(sockfd);
    abort();
}

void write_contents_to_file(char* file_name, char* buffer, int len) {
    FILE* fp = fopen(file_name, "w");
    if (fp == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
    size_t bytes_written = fwrite(buffer, 1, len, fp);
    if (bytes_written < len) {
        perror("Failed to write full buffer to file");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

// Given a time (in seconds) it pauses the program
void wait(unsigned int seconds) {
    usleep(seconds * 1000000);
}

char* read_file(char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* buffer = (char*)malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';

    fclose(file);

    return buffer;
}

// Send bytes using a TCP connection
int send_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_sent = send(sockfd, buf, len, flags);
    if (bytes_sent == -1) {
        perror("error sending tcp content");
        return EXIT_FAILURE;
    }
    if (bytes_sent < len) {
        perror("Error sending all the bytes for tcp");
    }
    return bytes_sent;
}

// Send the contents of a file for TCP connections
void send_file_contents(int sockfd, char* file_path) {
    char* file_contents = read_file(file_path);
    int len = strlen(file_contents);
    if (send_bytes(sockfd, file_contents, len, 0) < 0) {
        handle_error(sockfd, "send()");
    }
    free(file_contents);
}

// Receive bytes for TCP
int receive_bytes(int sockfd, char *buf, int len, int flags) {
    ssize_t bytes_received = recv(sockfd, buf, len, flags);
    if (bytes_received == -1) {
        perror("recv()");
        return EXIT_FAILURE;
    } else if (bytes_received == 0) {
        return EOF;
    }
    return bytes_received;
}

const char* get_value(char* file_path, char* key) {
    json_t *root;
    json_error_t error;

    const char* text = read_file(file_path);
    root = json_loads(text, 0, &error);

    if (!root) {
        fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
        exit(EXIT_FAILURE);
    }
    
    if (!json_is_object(root)) {
         fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
         exit(EXIT_FAILURE);
    }

    json_t *data = json_object_get(root, key);

    // TODO: if key is invalid -> return a valid key from hashmap object (to be created)
    
    if (!json_is_string(data)) {
        fprintf(stderr, "error parsing key %s\n", key);
        exit(EXIT_FAILURE);
    }

    const char* value = json_string_value(data);

    return value;
}
