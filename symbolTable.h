#include "ast.h"

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

ht* return_function_ST();
STTreeNode* createSTTreeNode();
void addSTChild(STTreeNode *parent,STTreeNode *child);
STEntry* createSTEntry(STEntry* prevEntry);
STEntry* checkID(STTreeNode* node,char lexeme[]);
FunctionSTEntry* checkFunctionID(char lexeme[]);
STEntry* recursiveCheckID(STTreeNode* node,Token_Info* t);
ParamList* initialize_parameter_list();
void add_to_parameter_list(ParamList* pl, STEntry* st);
Type get_entry_type(enum Token tokenType);
int get_entry_width(Type type);
void populate_temporary_function_entry(Ast_Node* temp);
void generateST(STTreeNode* currSTNode, Ast_Node* root);
STTreeNode* generateSymbolTable(Ast_Node* ASTRoot);
void recursive_print_symbol_table(STTreeNode* root, FILE* fp);
void print_symbol_table(STTreeNode* root, FILE* fp);
void print_activation_record();
void recursive_print_static_dynamic_arrays(STTreeNode* root);
void print_static_dynamic_arrays(STTreeNode* root);

#endif