#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "helpers.h"


short pathExists(char * path){
    if(access(path,F_OK) == 0){
        return 1;
    }
    return 0;

}


char * request_reciever(int client_fd) {    
    // set a timeout of 2 seconds to the socket
    // this is for handling if the data is a multiple of 1024 which can't be handled by checking the received bytes
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    // read the request and save through dynamic memory allocation
    int buffer_size = 1024;
    char * request = (char *) malloc(buffer_size);
    int bytes_recieved = 0;
    int total_bytes = 0;
    while (1) {
    // //printf("Bytes recieved: %d\n", bytes_recieved);
    // //printf("request: %s\n", request);
    if (bytes_recieved >= 1024) {
        // we have reached the end of the buffer and we need to reallocate memory
        request = (char *) realloc(request, buffer_size + 1024);
        buffer_size += 1024;
    }
    int chunk_size = recv(client_fd, request + bytes_recieved, buffer_size - bytes_recieved, 0);
    // //printf("Chunk size: %d\n", chunk_size);
    if (chunk_size < buffer_size - bytes_recieved) {
        break;
    }

    bytes_recieved += chunk_size;
    }

    return request;
}