#include "symbolTable.h"
#include "semanticError.h"

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

#endif
