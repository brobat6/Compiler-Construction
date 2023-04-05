/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "linkedList.h"

#ifndef _GRAMMAR_SYMBOL_H
#define _GRAMMAR_SYMBOL_H

typedef struct GRAMMAR_SYMBOL grammar_symbol;
struct GRAMMAR_SYMBOL {
    int epsilon, terminal, non_terminal;
    char* name;
    int start, end;
    unsigned long long* first_set;
    unsigned long long* follow_set;
    llNode** occurences;  // occurences of grammar symbol on rhs
    int occurence_count;
    int* occurence_rule;  // rule numbers corresponding to above occurences
};

grammar_symbol* initialize_grammar_symbol (grammar_symbol* cur_node, int name_size);

void deallocateGrammarSymbol (grammar_symbol* symbol);

#endif