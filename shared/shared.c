#include "shared.h"

#define MAX_KEY_LEN 100


void handle_error(int sockfd, char* error_msg) {
    perror(error_msg);
    close(sockfd);
    abort();
}

// Given a time (in seconds) it pauses the program
void wait(unsigned int count_down_time_in_secs) {
    struct timespec start_time, current_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    while (true) {
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        double elapsed = (current_time.tv_sec - start_time.tv_sec);
        elapsed += (current_time.tv_nsec - start_time.tv_nsec) / 0x3B9ACA00;
        
        if (elapsed >= count_down_time_in_secs) {
            printf("Time limit reached. Server stopping.\n");
            break;
        }
    
    }

}

   /* unsigned int x_hours = 0;
    unsigned int x_minutes = 0;
    unsigned int x_seconds = 0;
    unsigned int x_milliseconds = 0;
    unsigned int total_time = 0, time_left = 0;

    clock_t start_time, x_count_time;

   start_time = clock();
   time_left = count_down_time_in_secs - x_seconds; // Update timer

   while (time_left > 0) {
       x_count_time = clock();
       x_milliseconds = x_count_time - start_time;
       x_seconds = (x_milliseconds / (CLOCKS_PER_SEC) - (x_minutes * 60));
       x_minutes = (x_milliseconds / (CLOCKS_PER_SEC)) / 60;
       x_hours = x_minutes / 60;

       time_left = count_down_time_in_secs - x_seconds;
   }*/
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
