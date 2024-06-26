#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PATH_PREFIX "../shared/" 

// JSON Parsing
const char* get_value(char* file_path, char *key);
void handle_key_error(int ret_val, char* key, char* file_name);

// MISC
void handle_error(int sockfd, char* error_msg);
void wait(unsigned int count_down_time_in_secs);
void print_time(struct timespec);

// File Parsing
char* read_file(char* file_path);
void write_contents_to_file(char* file_name, char* buffer, int len);
void send_file_contents(int sockfd, char* file_path);

// Socket Programming
int send_bytes(int sockfd, char *buf, int len, int flags);
int receive_bytes(int sockfd, char *buf, int len, int flags);
