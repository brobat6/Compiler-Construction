/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "linkedList.h"

llHead* initializeHead () {
    llHead* cur_head = (llHead*) malloc(sizeof(llHead));
    cur_head->first_node = NULL;
    cur_head->last_node = NULL;
    cur_head->count = 0;
    cur_head->data = NULL;
    cur_head->first_set[0] = 0ull;
    cur_head->first_set[1] = 0ull;
    cur_head->grammar_rule = -1;
    return cur_head;
}

llNode* initializeNode () {
    llNode* cur_node = (llNode*) malloc(sizeof(llNode));
    cur_node->next = NULL;
    cur_node->prev = NULL;
    cur_node->data = NULL;
    return cur_node;
}

llHead* insertAtEnd (llHead* head, llNode* node) {
    if (head->count == 0) {
        head->first_node = node;
        head->last_node = node;
        head->count++;
        return head;
    }
    head->last_node->next = node;
    node->prev = head->last_node;
    head->last_node = node;
    head->count++;
    return head;
}

void removeFromEnd (llHead* head) {
    if (head->count == 0) {
        printf("Nothing to remove!\n");
        return;
    }
    if (head->count == 1) {
        llNode* temp = head->last_node;
        temp->next = NULL;
        temp->prev = NULL;
        head->first_node = NULL;
        head->last_node = NULL;
        head->count--;
        free(temp);
        return;
    }
    llNode* temp = head->last_node;
    head->last_node = head->last_node->prev;
    head->last_node->next = NULL;
    temp->next = NULL;
    temp->prev = NULL;
    head->count--;
    free(temp);
}
