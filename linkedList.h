/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

typedef struct LL_NODE llNode;
struct LL_NODE {
    void* data;
    llNode* next;
    llNode* prev;
};

typedef struct LL_HEAD llHead;
struct LL_HEAD {
    void* data;
    llNode* first_node;
    llNode* last_node;
    int count; // Length of corresponding grammar rule
    unsigned long long first_set[2];
    int grammar_rule;
};

llHead* initializeHead ();

llNode* initializeNode ();

llHead* insertAtEnd (llHead* head, llNode* node);

void removeFromEnd (llHead* head);

#endif