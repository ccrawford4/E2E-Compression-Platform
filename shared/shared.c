#include <jansson.h>
#include <string.h>
#include <errno.h>

#define MAX_KEY_LEN 100


void handle_error(int sockfd, char* error_msg) {
    perror(error_msg);
    close(sockfd);
    abort();
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
    
    if (!json_is_string(data)) {
        fprintf(stderr, "error parsing key %s\n", key);
        exit(EXIT_FAILURE);
    }

    const char* value = json_string_value(data);

    return value;
}
