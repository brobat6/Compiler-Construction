#include "astDef.h"

#ifndef _AST_H
#define _AST_H

Token_Info* createHeapTokenInfo(Token_Info old_token);

Ast_Node* createASTNode();

void printASTNode(Ast_Node* root, FILE* f);

Ast_Node* generateAST(treeNode* curr, Ast_Node* prev);

#endif