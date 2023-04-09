#include "typeCheck.h"

Type checkType(Operator op, Type t1, Type t2){
    if(t1==TYPE_ERROR || t2==TYPE_ERROR) return TYPE_UNDEFINED;

    if(op==OP_ARITH){
        if(t1==t2 && (t1==TYPE_INTEGER || t1==TYPE_REAL)) return t1;
        else{
            printf("Expression types of LHS RHS (Arith) dont match\n");
            return TYPE_ERROR;
        }
    } 
    else if(op==OP_REL){
        if(t1==t2 && (t1==TYPE_INTEGER || t1==TYPE_REAL)) return TYPE_BOOLEAN;
        else{
            printf("Expression types of LHS RHS (Rel) dont match\n");
            return TYPE_ERROR;
        }
    }
    else if(op==OP_LOGIC){
        if(t1==t2 && t1==TYPE_BOOLEAN) return TYPE_BOOLEAN;
        else{
            printf("Expression types of LHS RHS (Bool) dont match\n");
            return TYPE_ERROR;
        }
    } else if(op==OP_ARR){ // Also add cases for Real and Boolean
        if(t1==t2 && t1==TYPE_INTEGER) return TYPE_INTEGER;
        else{
            printf("Expression types of LHS RHS (Arr) dont match\n");
            return TYPE_ERROR;
        }
    } else if(op==OP_ASSIGN){
        if(t1==t2) return TYPE_UNDEFINED;
        else{
            printf("Expression types of LHS RHS (Assign) dont match\n");
            return TYPE_ERROR;
        }
    }
    else return TYPE_ERROR;
}

Operator token_to_op(Token t){
    if(t==PLUS || t==MINUS ||t==MUL ||t==DIV) return OP_ARITH;
    if(t==LT || t==LE || t==GT || t==GE || t==EQ || t==NE) return OP_REL;
    if(t==AND || t==OR) return OP_LOGIC;
    if(t==ASSIGNOP) return OP_ASSIGN;
    return OP_ERROR;
}

Type typecheckdfs(Ast_Node* root){

    // ASSIGNMENT/ARITHMETIC/BOOLEAN/ARRAY TYPE CHECKING
    if(root->type==22){
        Type left_expr=typecheckdfs(root->child_1);
        Type right_expr=typecheckdfs(root->child_2);
        
        return checkType(OP_ASSIGN, left_expr, right_expr);
    }

    if(root->type==23 || root->type==41 || root->type==42){
        return typecheckdfs(root->child_1);
    } 
    
    if(root->type==35 || root->type == 38 || root->type==39 || root->type==43 || root->type==44) {
        if(root->child_2==NULL) {
            if(root->syn_next == NULL) {
                return typecheckdfs(root->child_1);
            }
            return typecheckdfs(root->syn_next);
        }
        Type left_expr = typecheckdfs(root->inh_1);
        Operator op = token_to_op(root->child_1->token_data->token);
        Type right_expr;
        if(root->syn_next == NULL) {
            right_expr = typecheckdfs(root->child_2);
        } else {
            right_expr = typecheckdfs(root->syn_next);
        }
        return checkType(op, left_expr, right_expr);
    }

    if(root->type==36){
        if(root->child_2==NULL){
            return typecheckdfs(root->child_1);
        } else {
            return typecheckdfs(root->child_2);
        }
    }

    else if(root->type==37){
        Type left_expr=typecheckdfs(root->inh_1);
        Type right_expr=typecheckdfs(root->child_2);
        Operator op=token_to_op(root->child_1->token_data->token);

        //token to op
        return checkType(op, left_expr, right_expr);
    }

    else if(root->type==40){
        if(root->child_2==NULL){
            return typecheckdfs(root->child_1);
        } else{
            // Type checking will happen at <N11>. 
            return typecheckdfs(root->child_2);
        }
    }

    else if(root->type == 31) {
        // <N11>, type checking.
        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr = typecheckdfs(root->child_1);
        
        return checkType(OP_ARR, left_expr, right_expr);
    }

    else if(root->type==0){
        if(root->token_data->token == ID) {
            // Do this later with symbol table information.
            return TYPE_INTEGER;
        }
        if(root->token_data->token == NUM) {
            return TYPE_INTEGER;
        }
        if(root->token_data->token == RNUM) {
            return TYPE_REAL;
        }
        if(root->token_data->token == BOOLEAN) {
            return TYPE_BOOLEAN;
        }
        return TYPE_UNDEFINED;
    }

    else{
        Type t;
        if(root->child_1!=NULL) t=typecheckdfs(root->child_1);
        if(root->child_2!=NULL) t=typecheckdfs(root->child_2);
        if(root->child_3!=NULL) t=typecheckdfs(root->child_3);
        if(root->child_4!=NULL) t=typecheckdfs(root->child_4);
        if(root->child_5!=NULL) t=typecheckdfs(root->child_5);
        if(root->syn_next!=NULL) t=typecheckdfs(root->syn_next);

        return TYPE_UNDEFINED;
    }
}