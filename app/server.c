#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "hashtable.h"
#include "parsers.h"
#include "helpers.h"

int main() {
	// Disable output buffering
	setbuf(stdout, NULL);
 	setbuf(stderr, NULL);

	// You can use print statements as follows for debugging, they'll be visible when running tests.
	printf("Logs from your program will appear here!\n");

	// Uncomment this block to pass the first stage
	//
	int server_fd, client_addr_len;
	struct sockaddr_in client_addr;
	

	// Create a socket file descriptor
	// AF_INET is the address family used for IPv4 addresses
	// SOCK_STREAM is the socket type for TCP connections
	// 0 is the protocol value for IP

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1) {
		printf("Socket creation failed: %s...\n", strerror(errno));
		return 1;
	}
	
	// Since the tester restarts your program quite often, setting SO_REUSEADDR
	// ensures that we don't run into 'Address already in use' errors
	int reuse = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
		printf("SO_REUSEADDR failed: %s \n", strerror(errno));
		return 1;
	}

	// creates a socket address structure
	// htons and htonl convert the port number and IP address to network byte order
	// INADDR_ANY means that the socket will listen on all network interfaces 0.0.0.0
	// AF_INET is the address family used for IPv4 addresses
	
	struct sockaddr_in serv_addr = { .sin_family = AF_INET ,
									 .sin_port = htons(4221),
									 .sin_addr = { htonl(INADDR_ANY) },
									};
	
	if (bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0) {
		printf("Bind failed: %s \n", strerror(errno));
		return 1;
	}
	

	// The listen system call allows the process to listen on the socket for connections
	// If a connection request arrives when the queue is full, the client may receive an error
	// The backlog argument defines the maximum length to which the queue of pending connections for server_fd may grow

	int connection_backlog = 5;
	if (listen(server_fd, connection_backlog) != 0) {
		printf("Listen failed: %s \n", strerror(errno));
		return 1;
	}
	
	printf("Waiting for a client to connect...\n");
	client_addr_len = sizeof(client_addr);
	

	// for each client we have a file descriptor that we will be using to communicate with the client
	// accept system call returns a new socket file descriptor for the accepted connection

	int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_len);

	// log the client's IP address and port

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(client_addr.sin_addr), ip, INET_ADDRSTRLEN); // convert IP address to string
	printf("Connection from %s:%d\n", ip, ntohs(client_addr.sin_port));

	char * request = request_reciever(client_fd);
	printf("Request: %s\n", request);
	hash_table * request_table = request_parser(request);

	// check if path is valid
	char * path = GetItem(request_table,"path");
	short path_exists = pathExists(path);
	if (path_exists == 0){
		char * response = "HTTP/1.1 404 Not Found\r\n\r\n";
		int bytes_sent_num = send(client_fd, response, strlen(response), 0);
	}
	else{
		char *response = "HTTP/1.1 200 OK\r\n\r\n";
		int bytes_sent_num = send(client_fd, response, strlen(response), 0);
	}



	free(request);
	close(server_fd);
	return 0;
}




