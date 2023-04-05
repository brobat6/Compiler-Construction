/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "grammarSymbol.h"
#include "lexerDef.h"

#ifndef _TREE_H
#define _TREE_H

typedef struct treeNode {
  grammar_symbol* symbol;
  struct token_info token;
  struct treeNode* firstchild;
  struct treeNode* lastchild;
  struct treeNode* next;
  struct treeNode* prev;
  struct treeNode* parent;
} treeNode;

treeNode* createTreeNode(grammar_symbol * gs, struct token_info token);

void addChild(treeNode *parent, treeNode *child);

treeNode* nextLeaf (treeNode* cur_node);

void deallocateTree(treeNode* root);

#endif