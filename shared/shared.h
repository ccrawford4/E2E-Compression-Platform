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

#define PATH_PREFIX "../shared/" 
const char* get_value(char* file_path, char* key);
char* read_file(char* file_path);
void handle_error(int sockfd, char* error_msg);
void wait(unsigned int count_down_time_in_secs);
void write_contents_to_file(char* file_name, char* buffer, size_t buffer_size);
int send_bytes(int sockfd, char* buf, int len, int flags);
void send_file_contents(int sockfd, char* file_path);
