#include "astDef.h"

#ifndef _AST_H
#define _AST_H

Token_Info* createHeapTokenInfo(Token_Info old_token);

Ast_Node* createASTNode();

void printASTNode(Ast_Node* root);

void printASTNodeSTDOUT(Ast_Node* root, Ast_Node* prev, FILE* fp);

void traverseAST(Ast_Node* root, Ast_Node* prev, FILE* fp);

Ast_Node* generateAST(treeNode* curr, Ast_Node* prev);

Ast_Node* wrapper_create_AST(treeNode* parse_root);

int get_total_ast_memory();

#endif