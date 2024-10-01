#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>


// hash table item
typedef struct ht_item{
    char * key;
    char * value;
} ht_item;



// hashtable stuct that contains an array of hashtable items
typedef struct hash_table{
    struct ht_item ** items;
    int size;
    int count;
} hash_table;


hash_table* CreateHashTable(int size){
    hash_table * new_ht = (hash_table *) malloc(sizeof(hash_table));
    new_ht->size = size;
    new_ht->count = 0;
    new_ht->items = (ht_item**) calloc(new_ht->size,sizeof(ht_item*));

    for(int i = 0 ; i < size ;i++){
        new_ht->items[i] = NULL;
    }

    return new_ht;

}


ht_item* CreateItem(char * key, char * value) {
    ht_item* new_ht_item = (ht_item*) malloc(sizeof(ht_item));
    // Add 1 for the null terminator
    new_ht_item->key = (char *) malloc(strlen(key) + 1);
    new_ht_item->value = (char *) malloc(strlen(value) + 1);
    strcpy(new_ht_item->key, key);
    strcpy(new_ht_item->value, value);
    return new_ht_item;
}

unsigned int HashFunction(char * str,int max_capacity){
    unsigned int value = 0 ;

    for(int i = 0; str[i] ;i++){
        value += str[i];
    }
    return value % max_capacity;
}

void InsertItem(hash_table* table, char * key , char * value){
    int hash = HashFunction(key,table->size);
    // we will apply linear probing to handle collision
    //printf("Hash: %d\n", hash);
    // create an item
    ht_item* item = CreateItem(key,value);
    //printf("Item created\n");
    // get the current item at the hash index
    ht_item* current_item = table->items[hash];
    // if the current item is null, insert the new item
    if(current_item == NULL){
        table->items[hash] = item;
        table->count++;
    }
    else{
        // if the current item is not null, we check if the key is the same
        if(strcmp(current_item->key,key) == 0){
            // if the key is the same, we update the value
            free(table->items[hash]->value);
            table->items[hash]->value = (char *) malloc(sizeof(value) + 1);
            strcpy(value,table->items[hash]->value);
        }
        else{
            // if the key is not the same, we apply linear probing
            int i = 1;
            while(1){
                // go to the next spot and check if it is null or not 
                int new_hash = (hash + i) % table->size;
                ht_item* new_item = table->items[new_hash];
                // if null insert the new item, else check if the key is the same and update the value if not go to the next spot
                if(new_item == NULL){
                    table->items[new_hash] = item;
                    table->count++;
                    break;
                }
                else{
                    if(strcmp(new_item->key,key) == 0){
                        free(table->items[new_hash]->value);
                        table->items[new_hash]->value = (char *) malloc(sizeof(value) + 1);
                        strcpy(value,table->items[new_hash]->value);
                        break;
                    }
                }
                i++;
            }
        }
    }


}

char * SearchItem(hash_table* table, char * key){
    // first lets compute the hash
    int hash = HashFunction(key,table->size);
    // get the current item at the hash index
    ht_item* current_item = table->items[hash];
    // if the current item is null, return null
    if(current_item == NULL){
        return NULL;
    }
    // if the key is the same, return the value
    if(strcmp(current_item->key,key) == 0){
        return current_item->value;
    }
    else{
        // if the key is not the same, we apply linear probing
        int i = 1;
        while(1){
            // go to the next spot and check if it is null or not 
            int new_hash = (hash + i) % table->size;
            ht_item* new_item = table->items[new_hash];
            // if null this means the key is not in the table
            if(new_item == NULL){
                return NULL;
            }
            else{
                if(strcmp(new_item->key,key) == 0){
                    return new_item->value;
                }
            }
            i++;
        }
    }
}

void DeleteItem(hash_table* table, char * key){
    
    // first lets compute the hash 
    int hash = HashFunction(key,table->size);

    // get the current item at the hash index
    ht_item* current_item = table->items[hash];

    // if the current item is null, return
    if(current_item == NULL){
        return;
    }
    // if the key is the same, we delete the item
    if(strcmp(current_item->key,key) == 0){
        free(table->items[hash]->key);
        free(table->items[hash]->value);
        free(table->items[hash]);
        table->items[hash] = NULL;
        table->count--;
    }
    else{
        // if the key is not the same, we apply linear probing
        int i = 1;
        while(1){
            // go to the next spot and check if it is null or not 
            int new_hash = (hash + i) % table->size;
            ht_item* new_item = table->items[new_hash];
            // if null this means the key is not in the table
            if(new_item == NULL){
                break;
            }
            else{
                if(strcmp(new_item->key,key) == 0){
                    free(table->items[new_hash]->key);
                    free(table->items[new_hash]->value);
                    free(table->items[new_hash]);
                    table->items[new_hash] = NULL;
                    table->count--;
                    break;
                }
            }
            i++;
        }
    }

}

void PrintTable(hash_table* table){
    for(int i = 0 ; i < table->size ;i++){
        if(table->items[i] != NULL){
            printf("%s: %s\n",table->items[i]->key,table->items[i]->value);
        }
    }
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



short pathExists(char * path){
    if(access(path,F_OK) == 0){
        return 1;
    }
    return 0;

}