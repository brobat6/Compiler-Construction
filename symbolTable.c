#include "symbolTable.h"

#define DATASEGMENTOFFSET 1000
int curOffset=DATASEGMENTOFFSET;
STTreeNode* curSTTreeNode=NULL;
STTreeNode* globalSTRoot=NULL;
STEntry* tempEntry;

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

STEntry* createSTEntry(STEntry* prevEntry) {
    STEntry* newNode = NULL;
    if(prevEntry != NULL) {
        newNode = (STEntry*)malloc(sizeof(STEntry));
        strcpy(newNode->variableName, prevEntry->variableName);
        newNode->type = prevEntry->type;
        newNode->isArray = prevEntry->isArray;
        newNode->isDynamic = prevEntry->isDynamic;
        newNode->range = prevEntry->range;
        newNode->declarationLineNumber = prevEntry->declarationLineNumber;
        newNode->width = prevEntry->width;
        newNode->offset = prevEntry->offset;
    }
    return newNode;
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

void throw_already_exists_error(Token_Info* token) {
    printf("symbol table error at line %d\n", token->lineNumber);
}

void generateST(STTreeNode* currSTNode, Ast_Node* root, Ast_Node* prev) {
    if(root->type == 49) {
        // WHILE statement.
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
        return;
    }
    if(root->type == 46) {
        // Conditional statement (SWITCH)
        int start_line_no = root->child_1->token_data->lineNumber;
        int end_line_no = root->child_5->token_data->lineNumber;
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, currSTNode->moduleName);
        childSTNode->offset = curOffset;
        generateST(childSTNode, root->child_3, root);
        generateST(childSTNode, root->child_4, root);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    if(root->type == 5) {
        // driver module
        // REMEMBER TO ALSO ADD ENTRY IN MODULE SYMBOL TABLE!!!!
        int start_line_no = root->token_data->lineNumber; // DRIVERDEF
        int end_line_no = root->child_1->child_3->token_data->lineNumber; // END
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, "driver");
        childSTNode->offset = curOffset;
        generateST(childSTNode, root->child_1, root);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    if(root->type == 6) {
        // module

    }
    if(root->type == 45) {
        Ast_Node* temp = root->child_1;
        if(temp->child_2 == NULL) {
            enum Token tokenType = temp->child_1->token_data->token;
            if(tokenType == NUM) {
                tempEntry->type = TYPE_INTEGER;
            } else if(tokenType == RNUM) {
                tempEntry->type = REAL;
            } else if(tokenType == BOOLEAN) {
                tempEntry->type = BOOLEAN;
            } else {
                assert(false); // this should never happen.
            }
            tempEntry->isArray = false;
        } else {
            enum Token tokenType = temp->child_2->token_data->token;
            if(tokenType == NUM) {
                tempEntry->type = TYPE_INTEGER;
            } else if(tokenType == RNUM) {
                tempEntry->type = REAL;
            } else if(tokenType == BOOLEAN) {
                tempEntry->type = BOOLEAN;
            } else {
                assert(false); // this should never happen.
            }
            tempEntry->isArray = true;
            temp = temp->child_1;
            // Lower range
            int temp_sign = 1;
            if(temp->child_1->child_1->token_data->token == MINUS) {
                temp_sign = -1;
            }
            if(temp->child_1->child_2->token_data->token == NUM) {
                tempEntry->isDynamic.lower = false;
                tempEntry->range.lower.value = temp->child_1->child_2->token_data->value.num * temp_sign;
            } else {
                tempEntry->isDynamic.lower = true;
                strcpy(tempEntry->range.lower.lexeme, temp->child_1->child_2->token_data->lexeme);
                tempEntry->range.lower_sign = temp_sign;
            }
            // Upper range
            temp_sign = 1;
            if(temp->child_2->child_1->token_data->token == MINUS) {
                temp_sign = -1;
            }
            if(temp->child_2->child_2->token_data->token == NUM) {
                tempEntry->isDynamic.upper = false;
                tempEntry->range.upper.value = temp->child_2->child_2->token_data->value.num * temp_sign;
            } else {
                tempEntry->isDynamic.upper = true;
                strcpy(tempEntry->range.upper.lexeme, temp->child_2->child_2->token_data->lexeme);
                tempEntry->range.upper_sign = temp_sign;
            }
        }
        temp = root->child_2;
        assert(temp != NULL); // i.e., idList is not empty.
        while(temp != NULL) {
            STEntry* newEntry = createSTEntry(tempEntry);
            strcpy(newEntry->variableName, temp->child_1->token_data->lexeme);
            newEntry->declarationLineNumber = temp->child_1->token_data->lineNumber;
            if(newEntry->type == TYPE_INTEGER) {
                newEntry->width = 2;
            } else if(newEntry->type == TYPE_REAL) {
                newEntry->width = 4;
            } else if(newEntry->type == TYPE_BOOLEAN) {
                newEntry->width = 8;
            }
            if(newEntry->isArray == true) {
                if(newEntry->isDynamic.lower == false && newEntry->isDynamic.upper == false) {
                    newEntry->width *= (newEntry->range.upper.value - newEntry->range.lower.value + 1);
                    newEntry->width++; // Because apparently, array name also takes 1 byte.
                } else {
                    newEntry->width = 0; // NEED TO CLARIFY THIS. DYNAMIC ARRAY!!!!! WIDTH, OFFSET HOW???
                }
            } 
            newEntry->offset = curOffset;
            curOffset += newEntry->width;
            if(checkID(currSTNode, temp->child_1->token_data->lexeme) != NULL) {
                // ERROR!!!!! 
                throw_already_exists_error(temp->child_1->token_data);
            } else {
                ht_store(currSTNode->hashTable, newEntry->variableName, newEntry);
            }
            temp = temp->syn_next;
        }
        return;
    }
}

STTreeNode* generateSymbolTable(Ast_Node* ASTRoot){      ////confirm input parameter; INCOMPLETE
    STTreeNode* STRoot = createSTTreeNode();
    globalSTRoot=STRoot;
    STRoot->nestingLevel=-1;
    tempEntry = (STEntry*)malloc(sizeof(STEntry));
    strcpy(STRoot->moduleName,"ROOT");



}

void printSymbolTable(){            ////INCOMPLETE
    STTreeNode* temp=globalSTRoot;
    
}

