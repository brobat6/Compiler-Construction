/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "parserDef.h"

#ifndef _PARSER_H
#define _PARSER_H

void readGrammarFile ();

void findFirstSet (char* cur_id);

void findFollowSet (char* cur_id);

void computeFirstAndFollowSets ();

void createParseTable ();

void deallocateParseTable ();

void deallocateHead (llHead* head);

void deallocateNode (llNode* node);

void deallocateGrammar ();

treeNode* parseInputSourceCode (ht* lookup_table, FILE* fp, bool printError);

void printTreeNode(treeNode* treenode, FILE* tree_fp);

void printParseTree(treeNode* root, char* outputfile);

void deallocateFromParser ();

int number_of_parsing_errors();

#endif