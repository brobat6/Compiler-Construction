#include "symbolTable.h"

#define DATASEGMENTOFFSET 1000
int curOffset=DATASEGMENTOFFSET;
STTreeNode* curSTTreeNode=NULL;
STTreeNode* globalSTRoot=NULL;
enum Type currType = 3;

STTreeNode* createSTTreeNode(){             ///can use current AST node for data;
    STTreeNode* newNode = (STTreeNode*)malloc(sizeof(STTreeNode));
    newNode->hashTable=init_ht();
    newNode->offset=curOffset;
    newNode->nodeWidth = 0;
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

STEntry* checkID(STTreeNode* node,char lexeme[]){      //confirm input parameter
    return ht_fetch(node->hashTable,lexeme);
}

STEntry* recursiveCheckID(STTreeNode* node,Token_Info* t){         //confirm input parameter
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

void generateST(STTreeNode* currSTNode, Ast_Node* root, Ast_Node* prev) {
    if(root->type == 49) {
        int start_line_no = root->token_data->lineNumber;
        int end_line_no = root->child_4->token_data->lineNumber;
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, currSTNode->moduleName);
        childSTNode->offset = curOffset;
        generateST(childSTNode, root->child_3, root);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
    }
    if(root->type == 45) {
        enum Token tokenType = root->child_1->token_data->token;
        if(tokenType == NUM) {
            currType = INTEGER;
        } else if(tokenType == RNUM) {
            currType = REAL;
        } else if(tokenType == BOOLEAN) {
            currType = BOOLEAN;
        } else {
            assert(false); // this should never happen.
        }
    }
}

STTreeNode* generateSymbolTable(Ast_Node* ASTRoot){      ////confirm input parameter; INCOMPLETE
    STTreeNode* STRoot = createSTTreeNode();
    globalSTRoot=STRoot;
    STRoot->nestingLevel=-1;
    currType = 3;
    strcpy(STRoot->moduleName,"ROOT");



}

void printSymbolTable(){            ////INCOMPLETE
    STTreeNode* temp=globalSTRoot;
    
}

