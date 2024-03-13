#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_PREFIX "../shared/" 
const char* get_value(char* file_path, char* key);
char* read_file(char* file_path);
void handle_error(int sockfd, char* error_msg);
