#include "shared.h"
#define NUM_ITEMS 11
#define ERROR "ERROR"

// Data entry for each default JSON item
typedef struct {
    char* key;
    const char* value;
} item;

// Object to keep track of default items
item default_items[NUM_ITEMS] = {
    {"UDP_src_port_number", "9876"},
    {"UDP_dest_port_number", "8765"},
    {"TCP_HEADSYN_dest_port_number", "9999"},
    {"TCP_TAILSYN_dest_port_number", "8888"},
    {"TCP_PREPROB_port_number", "7777"},
    {"TCP_POSTPROB_port_number", "6666"},
    {"UDP_payload_size", "1000B"},
    {"measurement_time", "15"},
    {"UDP_packet_train_size", "6000"},
    {"UDP_packet_TTL", "225"},
    {"server_wait_time", "5"}
};

// Handles key errors if the key parsed is not valid
void handle_key_error(int ret_val, char* key, char* file_name) {
    if (key == 0) {
        printf("ERROR! Invalid Value For Said Key: %s\n", key);
        printf("See file %s\n", file_name);
        exit(EXIT_FAILURE);
    }
}

// Return the default value if the value doesn't in the JSON
const char* get_default(char* key) {
    for (int i = 0; i < NUM_ITEMS; i++) {
        if (!strcmp(default_items->key, key)) {
            return default_items->value;
        }
    }
    // Returns an ERROR string if the key could not be found
    return ERROR;
}

// Gets the JSON's corresponding value given a key
const char* get_value(char* file_path, char* key) {
    // Text points to the config file contents in memory
    const char* text = read_file(file_path); 

    // Create jansson json_t typedef objects
    json_t *root;
    json_error_t error;

    // Load the text into the json_t object
    root = json_loads(text, 0, &error);

    // Account for parsing errors - as per jansson API reference
    if (!root) {
        fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
        exit(EXIT_FAILURE);
    }

    if (!json_is_object(root)) {
         fprintf(stderr, "error on line %d: %s\n", error.line, error.text);
         exit(EXIT_FAILURE);
    }

    // Get the value when given a key
    json_t *data = json_object_get(root, key);
    const char* value;
    
    // If the value does not exist in the config file then try to get the default value
    if (!json_is_string(data)) { 
        value = get_default(key);
        if (!strcmp(value, ERROR)) {
            fprintf(stderr, "error parsing key %s\n", key);
            exit(EXIT_FAILURE);
        }
        return value;
    }

    // Get the string version of the value
    value = json_string_value(data);

    return value;
}
