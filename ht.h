/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "htDef.h"

#ifndef HT_H
#define HT_H

//list of functions
void exit_nomem(void);

ht* init_ht(void);

void delete_ht(ht* tab);

size_t ht_size(ht* tab);

ht_itr ht_iterator(ht* tab);

uint32_t generate_hashkey(const char* key);

void* ht_fetch(ht* tab, const char* key);

const char* ht_store_entry(ht_entry* entry, size_t cap, const char* key, void* data, size_t* cnt);

bool ht_resize(ht* tab);

const char* ht_store(ht* tab, const char* key, void* data);

bool ht_next_entry(ht_itr* itr);

int ht_fetch_index(ht* tab, const char* key);

#endif