#include "parser.h"

#ifndef _SYMBOLTABLEDEF_H
#define _SYMBOLTABLEDEF_H

typedef struct STTreeNode{
    ht* hashTable;
    int nestingLevel;
    char moduleName[21];
    struct{
        int begin;
        int end;
    }lineNumber;
    int offset;
    int nodeWidth;
    struct STTreeNode* parent;
    struct STTreeNode* leftMostChild;
    struct STTreeNode* rightMostChild;
    struct STTreeNode* sibling;
}STTreeNode; 

typedef enum Type{
    TYPE_INTEGER,
    TYPE_REAL,
    TYPE_BOOLEAN,
    TYPE_UNDEFINED,
    TYPE_ERROR
}Type; 

typedef struct STEntry{
    char variableName[21];
    enum Type type;
    bool isArray;
    struct{
        bool lower;
        bool upper;
    }isDynamic;
    struct{
        union{
            int value;
            char lexeme[21];
        }lower;
        union{
            int value;
            char lexeme[21];
        }upper;
    }range;
    int declarationLineNumber;
    int width;
    int offset;
}STEntry;

typedef struct paramListNode{
    STEntry* entry;
    struct parameterListNode* next; 
}paramListNode;

typedef struct paramList{
    int size;
    paramListNode* first;
    paramListNode* last;
}paramList;

typedef struct FunctionSTEntry{
    char moduleName[21];
    bool declared;
    bool defined;
    paramList* inputParamList;
    paramList* outputParamList;
}FunctionSTEntry;

#endif