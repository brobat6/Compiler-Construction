#include "ast.h"

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

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
void throw_function_already_exists_error(char module_name[], int line_no);
void throw_already_exists_error(char module_name[], int line_no);
void throw_function_not_declared_error(char module_name[], int line_no);
void generateST(STTreeNode* currSTNode, Ast_Node* root);
STTreeNode* generateSymbolTable(Ast_Node* ASTRoot);
void recursive_print_symbol_table(STTreeNode* root, FILE* fp);
void print_symbol_table(STTreeNode* root, FILE* fp);

void push_function_record(char module_name[]);
void pop_function_record();
bool function_is_recursive(char module_name[]);
void throw_function_is_recursive_error(char module_name[], int line_no);
void throw_error();
void functionChecker(Ast_Node* root);
void semanticAnalyzer(Ast_Node* root);

#endif