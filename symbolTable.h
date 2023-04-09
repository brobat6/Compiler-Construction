#include "ast.h"

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

STTreeNode* createSTTreeNode();

void addSTChild(STTreeNode *parent,STTreeNode *child);

STEntry* createSTEntry(STEntry* prevEntry);

STEntry* checkID(STTreeNode* node,char lexeme[]);

STEntry* recursiveCheckID(STTreeNode* node,Token_Info* t);

void generateST(STTreeNode* currSTNode, Ast_Node* root, Ast_Node* prev);

STTreeNode* generateSymbolTable(Ast_Node* ASTRoot);

void printSymbolTable();

#endif