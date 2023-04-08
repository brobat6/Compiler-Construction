#include "parser.h"

#ifndef _ASTDEF_H
#define _ASTDEF_H

typedef struct AST_NODE Ast_Node;
struct AST_NODE {
    int type;
    Ast_Node* syn_next; // Useful if the node is a part of a linkedlist. If it is not, or it is the end, make it NULL. Used for syn attributes.
    Ast_Node* inh_1; // ^^^^ Above attribute might be useless. Used in Rule 40.
    Token_Info* token_data; // Useful for nodes of the form <non-terminal> --> terminal. Helps with compression.
    Ast_Node* child_1; // In case of linked list, child_1 contains data of the node.
    Ast_Node* child_2;
    Ast_Node* child_3;
    Ast_Node* child_4;
    Ast_Node* child_5; // Parameters of the node. Consider renaming them to something better, or adding more parameters to help with readability.
};

extern const char* const ast_node_id[];

#endif