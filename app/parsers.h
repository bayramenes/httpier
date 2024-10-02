#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "hashtable.h"



hash_table* request_parser(char * request){
	// we will create a hash table to store the request line and the request headers
	// note that we cant use strtok to parse the request line and the headers because it will move to the next token making it not possible to parse the headers


	hash_table * request_table = CreateHashTable(20);
	
	char * request_line = strtok(request,"\r\n");
	// we will parse the first line
	if (request_line != NULL){
		int len = strlen(request_line); // we have to store it before hand because we will be changing the string
		for(int i = 0 ; i < len; i++){
			if(request_line[i] == ' ' || request_line[i] == '\r'){
				request_line[i] = '\0';
			}
		}

		char * method = request_line;
		char * path = request_line + strlen(method) + 1;
		char * version = path + strlen(path) + 1;


		InsertItem(request_table,"method",method);
		InsertItem(request_table,"path",path);
		InsertItem(request_table,"version",version);
	}


	// we will parse the headers
	request_line = strtok(NULL,"\r\n");
	while (request_line != NULL){
		for(int i = 0 ; i < strlen(request_line); i++){
			if(request_line[i] == ':'){
				request_line[i] = '\0';
				break;
			}
		}
		char * key = request_line;
		char * value = key + strlen(key) + 1 + 1; // +1 for the colon and +1 for the space
		InsertItem(request_table,key,value);
		request_line = strtok(NULL,"\r\n");
	}
	return request_table;
}



