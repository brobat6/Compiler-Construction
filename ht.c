/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "ht.h"

void exit_nomem(void) {
    fprintf(stderr, "Out of memory!\n");
    exit(1);
}

ht* init_ht(void){
    //Allocate memory for the hashtable.
    ht* tab = malloc(sizeof(ht));
    if (tab == NULL) return NULL;

    //Initialize the hashtable.
    tab->cap = INIT_CAP;
    tab->size = 0;

    //Allocate memory for the entry buckets.
    tab->entry = calloc(tab->cap, sizeof(ht_entry));
    if (tab->entry == NULL){
        //If could not allocate, free table before returning.
        free(tab); 
        return NULL;
    }

    return tab;
}

void delete_ht(ht* tab) {
    //Free the keys first.
    if (tab == NULL) return;
    for (size_t i = 0; i < tab->cap; i++) {
        if (tab->entry[i].key) free((void *)tab->entry[i].key);
        if (tab->entry[i].data) free(tab->entry[i].data);
    }

    //Then free the entries.
    free(tab->entry);

    //Finally free the table.
    free(tab);
}

size_t ht_size(ht* tab) {
    //Returns the number of distinct entries in the hastable
    return tab->size;
}

ht_itr ht_iterator(ht* tab) {
    //Initialize an interator.
    ht_itr itr;

    //Reference the table and index to the iterator.
    itr._table = tab;
    itr._index = 0;

    return itr;
}

bool ht_next_entry(ht_itr* itr) {
    //Reference the table to the iterator.
    ht* tab = itr->_table;

    //Loop over all the entries till end of table.
    while (itr->_index < tab->cap) {
        size_t i = itr->_index;
        itr->_index++;
        // printf("%d, ", (int)i);

        //If next non-empty entry is obtained, update iterator and key value.
        if (tab->entry[i].key != NULL) {
            // printf("%d, :", (int)i);
            ht_entry entry = tab->entry[i];

            itr->key = entry.key;
            itr->data = entry.data;

            return true;
        }
    }

    //If there is no next entry return false.
    return false;
}

uint32_t generate_hashkey(const char* key) {
    //Initialize the hash value to the offset
    uint32_t hash = OFFSET;

    //Calculate the hash value for the key
    for (const char* p = key; *p; p++) {
        hash ^= (uint32_t)(unsigned char)(*p);
        hash *= PRIME;
    }

    return hash;
}

void* ht_fetch(ht* tab, const char* key) {
    //We mod the hash wrt hashtable capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(tab->cap));

    //We try to find our entry till we encounter an empty entry(linear probing).
    while (tab->entry[hk].key != NULL) {
        //Key is found and we return the data in the entry
        if (strcmp(key, tab->entry[hk].key) == 0) return tab->entry[hk].data;
        
        //Key wasn't in this entry so we move to the next entry using linear probing.
        hk++;

        //If we reach end of table, we jump to first entry using mod function.
        hk %= tab->cap;
    }

    //If entry corresponding to key is not found, we return NULL
    return NULL;
}

const char* ht_store_entry(ht_entry* entry, size_t cap, const char* key, void* data, size_t* cnt) {
    //We mod the hash wrt capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(cap));

    //We loop till we find an empty entry.
    while (entry[hk].key != NULL){
        //If the key already exists, we update the value.
        if (strcmp(key, entry[hk].key) == 0) {
            entry[hk].data = data;
            return entry[hk].key;
        }

        //If key is not found we continue searching by linear probing.
        hk++;
        hk %= cap;
    }

    //If we did not find the key, we copy and increase the count.
    if (cnt != NULL){
        key = strdup(key);
        if (key == NULL) return NULL;

        (*cnt)++;
    }

    //Then we allocate the new entry.
    entry[hk].key = (char*)key;
    entry[hk].data = data;

    return key;
}

bool ht_resize(ht* tab) {
    //We double the table size when we resize.
    size_t new_cap = tab->cap * 2;

    //This case handles overflow.
    if (new_cap < tab->cap) return false;

    //We make a new entries record nd allocate memory to it.
    ht_entry* new_entry = calloc(new_cap, sizeof(ht_entry));
    if (new_entry == NULL) return false;

    //We populate the new entries with the old entries suitable to the new size of the hashtable.
    for (size_t i = 0; i < tab->cap; i++) {
        ht_entry entry = tab->entry[i];
        if (entry.key != NULL) ht_store_entry(new_entry, new_cap, entry.key,entry.data, NULL);
    }

    //We free the memory the old entries were holding.
    free(tab->entry);

    //We reference the new entries and capacity to the hashtable.
    tab->entry = new_entry;
    tab->cap = new_cap;

    return true;
}

const char* ht_store(ht* tab, const char* key, void* data) {
    //We assert that if data is NULL we have made an error in code logic and must throw an error + abort.
    assert(data != NULL);
    if (data == NULL) return NULL;

    //If hashtable is filled beyond 50%, we double the size of the hashtable to reduce collisions and accomodate new entries.
    if (tab->size >= tab->cap / 2) {
        //If we fail to resize, return NULL to show we could not store the new entry.
        if (!ht_resize(tab)) return NULL;
    }

    //Once resized, we finally store the entry in the hash table.
    return ht_store_entry(tab->entry, tab->cap, key, data, &tab->size);
}

int ht_fetch_index(ht* tab, const char* key) {
    //We mod the hash wrt hashtable capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(tab->cap));

    //We try to find our entry till we encounter an empty entry(linear probing).
    while (tab->entry[hk].key != NULL) {
        //Key is found and we return the data in the entry
        if (strcmp(key, tab->entry[hk].key) == 0) return (int)hk;
        
        //Key wasn't in this entry so we move to the next entry using linear probing.
        hk++;

        //If we reach end of table, we jump to first entry using mod function.
        hk %= tab->cap;
    }

    //If entry corresponding to key is not found, we return NULL
    return -1;
}