#include "ast.h"

#ifndef _TCDEF_H
#define _TCDEF_H

typedef enum Operator{
    OP_ARITH,
    OP_REL,
    OP_LOGIC,
    OP_ASSIGN,
    OP_ARR,
    OP_ERROR
}Operator;

// typedef enum Type{
//     TYPE_INTEGER,
//     TYPE_REAL,
//     TYPE_BOOLEAN,
//     TYPE_UNDEFINED,
//     TYPE_ERROR
// }Type;

#endif