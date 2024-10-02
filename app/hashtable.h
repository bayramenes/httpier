#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef HASHTABLE_H
#define HASHTABLE_H

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



// PROTOTYPES
hash_table* CreateHashTable(int size);
ht_item* CreateItem(char * key, char * value);
unsigned int HashFunction(char * str,int max_capacity);
void InsertItem(hash_table* table, char * key , char * value);
char * GetItem(hash_table* table, char * key);
void DeleteItem(hash_table* table, char * key);
void PrintTable(hash_table* table);



#endif