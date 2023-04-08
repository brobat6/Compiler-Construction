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
    int width;
    int offset;
}STEntry;


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

void* recursiveCheckID(STTreeNode* node,char lexeme[]){         //confirm input parameter
    if(checkID(node,lexeme)==NULL&&node->parent!=NULL){
        return recursiveCheckID(node->parent,lexeme);
    }
    return checkID(node,lexeme);
}

enum Type getType(char lexeme[]){           ////check input parameter....lexeme[] or pointer to ASTNode
    STEntry* entry=recursiveCheckID(lexeme);
    if(entry!=NULL){
        return  entry->type;
    }
    return undefined;
}

STTreeNode* generateSymbolTable(ASTnode* ASTRoot){      ////confirm input parameter;
    STTreeNode* STRoot = createSTTreeNode();
    globalSTRoot=STRoot;
    STRoot->nestingLevel=-1;


}

void printSymbolTable(){
    STTreeNode* temp=globalSTRoot;
    
}