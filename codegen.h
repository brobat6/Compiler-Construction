#include "symbolTable.h"

#ifndef _CODEGEN_H
#define _CODEGEN_H

typedef struct StorageStructure {
    STEntry* storage;
    int capacity;
    int size;
} storageStructure;

void codegen (Ast_Node* root, FILE* fp_asm);

#endif