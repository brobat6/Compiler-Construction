#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define DATASEGMENTOFFSET 1000

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
    integer,
    real,
    boolean,
    undefined
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


int curOffset=DATASEGMENTOFFSET;
STTreeNode* curSTTreeNode=NULL;
STTreeNode* globalSTRoot=NULL;


STTreeNode* createSTTreeNode(){             ///can use current AST node for data;
    STTreeNode* newNode = (STTreeNode*)malloc(sizeof(STTreeNode));
    newNode->ht=init_ht();
    newNode->offset=curOffset;
    newNode->parent=NULL;
    newNode->leftMostChild=NULL;
    newNode->rightMostChild=NULL;
    newNode->sibling=NULL;
    return newNode;
}

void addSTChild(STTreeNode *parent,STTreeNode *child){
    if(parent->leftMostChild==NULL){
        parent->leftMostChild=child;
        parent->rightMostChild=child;
    }
    else{
        parent->rightMostChild->sibling=child;
        parent->rightMostChild=child;
    }
    child->parent=parent;
    child->nestingLevel=parent->nestingLevel+1;
}

void* checkID(STTreeNode* node,char lexeme[]){      //confirm input parameter
    return ht_fetch(node->hashTable,lexeme);
}

void* recursiveCheckID(STTreeNode* node,token_info* t){         //confirm input parameter
    char lexeme[21];
    strcpy(lexeme,t->lexeme);
    if(checkID(node,lexeme)==NULL&&node->parent!=NULL){
        return recursiveCheckID(node->parent,t);
    }
    else if(checkID(node,lexeme)!=NULL&&checkID(node,lexeme)->declarationLineNumber>t->lineNumber){
        return recursiveCheckID(node->parent,t);
    }
    return checkID(node,lexeme);
}

STTreeNode* generateSymbolTable(Ast_Node* ASTRoot){      ////confirm input parameter; INCOMPLETE
    STTreeNode* STRoot = createSTTreeNode();
    globalSTRoot=STRoot;
    STRoot->nestingLevel=-1;
    strcpy(STRoot->moduleName,"ROOT");



}

void printSymbolTable(){            ////INCOMPLETE
    STTreeNode* temp=globalSTRoot;
    
}

