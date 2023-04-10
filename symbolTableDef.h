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
    bool is_for_loop_variable;
    struct{
        bool lower;
        bool upper;
    }isDynamic;
    struct{
        union{
            int value;
            char lexeme[21];
        }lower;
        int lower_sign; // 1 or -1.
        union{
            int value;
            char lexeme[21];
        }upper;
        int upper_sign;
    }range;
    int declarationLineNumber;
    int width; // If any isDynamic is true, then this is not computed.
    int offset;
    bool isParameter; // Since nesting level for parameters has to be -1. (fuv)
}STEntry;

typedef struct ParamListNode {
    STEntry* entry;
    struct ParamListNode* next;
}ParamListNode;

typedef struct ParamList{
    int size;
    ParamListNode* first;
    ParamListNode* last;
}ParamList;

typedef struct FunctionSTEntry{
    char moduleName[21];
    int declaration_line_no;
    bool defined;
    int function_width;
    ParamList* inputParamList;
    ParamList* outputParamList;
}FunctionSTEntry;

#endif