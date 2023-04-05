/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "lexerDef.h"

#ifndef _LEXER_H
#define _LEXER_H

//list of functions
void fill_keyword(ht* lookup_table, char* keyword, Token t);

void fill_lookup_table(ht* lookup_table);

void print_lookup_table(ht* lookup_table);

void getStream(FILE *fp);

char nextChar(FILE* fp);

struct token_info tokenize(enum Token t,int begin,int end,int ln);

struct token_info IdNumTokenize(enum Token t,char* lexeme,int ln);

void retract(int count);

void movePointerPair(FILE* fp);

void moveLexemeBegin(FILE* fp);

void throwLE(int begin,int end, int lineNumber);

void throwLEOverflow(char* lexeme,int lineNumber);

struct token_info getNextToken(ht* lookup_table, FILE* fp);

int removeComments(FILE* fp);

void printLexemes(FILE* fp,int buff_size, ht* lookup_table);

void start_lexer(FILE* fp, int buff_size,bool pe);

void deallocateBuff ();

#endif