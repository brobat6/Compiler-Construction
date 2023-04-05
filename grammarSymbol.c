/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "grammarSymbol.h"

grammar_symbol* initialize_grammar_symbol (grammar_symbol* cur_node, int name_size) {
    cur_node = (grammar_symbol*) malloc(sizeof(grammar_symbol));
    cur_node->epsilon = 0;
    cur_node->terminal = 0;
    cur_node->non_terminal = 0;
    cur_node->name = (char*) calloc(name_size, sizeof(char));
    cur_node->start = -1;
    cur_node->end = -1;
    cur_node->first_set = NULL;
    cur_node->follow_set = NULL;
    cur_node->occurences = NULL;
    cur_node->occurence_count = 0;
    cur_node->occurence_rule = NULL;
    return cur_node;
}

void deallocateGrammarSymbol (grammar_symbol* symbol) {
    if (symbol == NULL) {
        return;
    }
    if (symbol->name != NULL) {
        free(symbol->name);
    }
    if (symbol->first_set != NULL) {
        free(symbol->first_set);
    }
    if (symbol->follow_set != NULL) {
        free(symbol->follow_set);
    }
    if (symbol->occurences != NULL) {
        free(symbol->occurences);
    }
    if (symbol->occurence_rule != NULL) {
        free(symbol->occurence_rule);
    }
    free(symbol);
}