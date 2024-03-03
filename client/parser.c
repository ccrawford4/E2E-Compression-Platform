#include "main.h"
#include <jansson.h>
#include <string.h>
#include <errno.h>

#define MAX_KEY_LEN 100

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

    char* text = read_file(file_path);

    root = json_loads(text, 0, &error);
    free(text);

    if (!root) {
        fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
        exit(EXIT_FAILURE);
    }

    if (!json_is_array(root)) {
        fprintf(stderr, "error: root is not an array\n");
        json_decref(root);
        exit(EXIT_FAILURE);
    }

    json_t *data, *value;
    data = json_array_get(root, 0);
    
    if (!json_is_object(data)) {
         fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
         exit(EXIT_FAILURE);
    }

    
    const char *ret_value;

    value = json_object_get(data, key);

    if (!json_is_string(value)) {
        fprintf(stderr, "error parsing key %s\n", key);
        exit(EXIT_FAILURE);
    }

    ret_value = json_string_value(value);

    return ret_value;
}
