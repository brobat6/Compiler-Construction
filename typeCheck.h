#include "typeCheckDef.h"

#ifndef _TC_H
#define _TC_H

Type checkType(Token_Info* tk_data, Operator op, Type t1, Type t2);

Operator token_to_op(Token t);

Type typecheckdfs(Ast_Node* root);

#endif
