#include "symbolTable.h"

typedef struct StorageStructure {
    STEntry* storage;
    int capacity;
    int size;
} storageStructure;

void codegen (Ast_Node* root);