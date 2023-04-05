/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#ifndef _HT_DEF_H
#define _HT_DEF_H

#define INIT_CAP 16
#define OFFSET 2166136261
#define PRIME 16777619

typedef struct{
    const char* key;
    void* data;
} ht_entry;

typedef struct{
    ht_entry* entry;
    size_t cap;
    size_t size;
} ht;

typedef struct{
    const char* key; 
    void* data;      

    //dont use directly
    ht* _table; 
    size_t _index;    
} ht_itr;

#endif