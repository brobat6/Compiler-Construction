#include "semanticError.h"

Error_List* error_list;

void add_error(Error error) {
    Error_List* new_error = (Error_List*)malloc(sizeof(Error_List));
    new_error->error = error;
    if(error_list == NULL) {
        error_list = new_error;
        error_list->prev = NULL;
        error_list->next = NULL;
        return;
    }
    if(error_list->error.line > new_error->error.line) {
        new_error->next = error_list;
        new_error->prev = NULL;
        error_list->prev = new_error;
        error_list = new_error;
        return;
    }
    Error_List* prev = error_list;
    Error_List* next = error_list->next;
    while(next != NULL && next->error.line <= new_error->error.line) {
        prev = prev->next;
        next = next->next;
    }
    if(next == NULL) {
        new_error->next = NULL;
        new_error->prev = prev;
        prev->next = new_error;
    } else {
        prev->next = new_error;
        new_error->next = next;
        next->prev = new_error;
        new_error->prev = prev;
    }
}

void print_semantic_errors(FILE *fp) {
    Error_List* e = error_list;
    while(e != NULL) {

        e = e->next;
    }
}