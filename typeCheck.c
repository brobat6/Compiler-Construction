#include "typeCheck.h"

Type checkType(Operator op, Type t1, Type t2){
    if(t1==TYPE_ERROR || t2==TYPE_ERROR || t1==TYPE_UNDEFINED|| t2==TYPE_UNDEFINED) return TYPE_UNDEFINED;

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

int boundCheck(Token_Info* tk_data, STEntry* st_entry, int index){
    int lower_bound=st_entry->range.lower.value;
    int upper_bound=st_entry->range.upper.value;

    if(index>=lower_bound && index<=upper_bound) return 1;
    else{
        printf("Line %d: Array %s out of bounds.\n", tk_data->lineNumber, tk_data->lexeme);
        return 0;
    }   
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

    if(root->type==34){
        Type right_expr=typecheckdfs(root->child_2);
        if(right_expr!=TYPE_INTEGER && right_expr!=TYPE_REAL) return TYPE_ERROR;
        else return right_expr;
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

    if(root->type==36 || root->type == 40){
        if(root->child_2==NULL){
            return typecheckdfs(root->child_1);
        } else {
            return typecheckdfs(root->child_2);
        }
    }

    if(root->type==37){
        Type left_expr=typecheckdfs(root->inh_1);
        Type right_expr=typecheckdfs(root->child_2);
        Operator op=token_to_op(root->child_1->token_data->token);

        return checkType(op, left_expr, right_expr);
    }

     if(root->type == 31) {
        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr = typecheckdfs(root->child_1);

        STEntry* st_entry=recursiveCheckID(root->symbol_table, root->inh_1->token_data);
        if(!st_entry->isArray || (left_expr!=TYPE_INTEGER && left_expr!=TYPE_REAL)) return TYPE_ERROR;
        else{
            if(!st_entry->isDynamic.lower && !st_entry->isDynamic.upper){
                int temp;
                
                if(root->child_1->type==42 && root->child_1->child_1->token_data->token==NUM){
                    temp = root->child_1->child_1->token_data->value.num;
                    temp*=-1;
                } else if(root->child_1->type==41 && root->child_1->child_1->token_data->token==NUM){
                    temp = root->child_1->child_1->token_data->value.num;
                } else if(root->child_1->type==43){
                    if(root->child_1->syn_next==NULL && root->child_1->child_1->syn_next==NULL) temp = root->child_1->child_1->child_1->token_data->value.num;
                    else{
                        Type index_expr = typecheckdfs(root->child_1);
                        if(index_expr==TYPE_INTEGER) return left_expr;
                        else return TYPE_ERROR;
                    }
                } else if(root->child_1->child_1->token_data->token!=NUM){
                    STEntry* st_id = recursiveCheckID(root->child_1->child_1->symbol_table, root->child_1->child_1->token_data);
                    if(st_id==NULL) return TYPE_ERROR;
                    if(st_id->type!=TYPE_INTEGER) return left_expr;
                    else return TYPE_UNDEFINED;
                }

                if(boundCheck(root->inh_1->token_data, st_entry, temp)) return left_expr;
                else{printf("Array out of bounds with index %d\n", temp); return TYPE_ERROR;}
            }
        }
    }

    if(root->type==21){
        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr = typecheckdfs(root->child_1);
        STEntry* st_entry=recursiveCheckID(root->symbol_table, root->inh_1->token_data);
        if(!st_entry->isArray || (left_expr!=TYPE_INTEGER && left_expr!=TYPE_REAL)) return TYPE_ERROR;
        else{
            if(!st_entry->isDynamic.lower && !st_entry->isDynamic.upper){
                int temp;

                if(root->child_1->child_1==NULL){
                    if(root->child_1->child_2->token_data->token==ID){
                        STEntry* st_id = recursiveCheckID(root->child_1->child_2->symbol_table, root->child_1->child_2->token_data);
                        if(st_id==NULL) return TYPE_ERROR;
                        if(st_id->type!=TYPE_INTEGER) return TYPE_ERROR;
                        else return TYPE_UNDEFINED;
                    }
                    else{
                        temp = root->child_1->child_2->token_data->value.num;
                    }
                } else if(root->child_1->child_1->token_data->token==MINUS && root->child_1->child_2->token_data->token==NUM){
                    temp = root->child_1->child_2->token_data->value.num;
                    temp*=-1;
                } else if(root->child_1->child_1->token_data->token==PLUS && root->child_1->child_2->token_data->token==NUM){
                    temp = root->child_1->child_2->token_data->value.num;
                } else if(root->child_1->child_2->token_data->token==ID){
                    STEntry* st_id = recursiveCheckID(root->child_1->child_2->symbol_table, root->child_1->child_2->token_data);
                    if(st_id==NULL) return TYPE_ERROR;
                    if(st_id->type!=TYPE_INTEGER) return left_expr;
                    else return TYPE_UNDEFINED;
                }

                printf("%d \n", temp);
                if(boundCheck(root->inh_1->token_data, st_entry, temp)) return TYPE_UNDEFINED;
                else{printf("Array out of bounds with index %d\n", temp); return TYPE_ERROR;}
            }
        }
    }

    //CONDITIONAL SWITCH-CASE


    if(root->type==0){
        if(root->token_data->token == ID) {
            return TYPE_INTEGER;

            // SEG FAULT DUE TO RECURSIVECHECKID FUNC -> SYMBOL tABLE AST NODE ME NAI AARI


            // printf("ok1\n");
            // printf("%d\n", root->symbol_table->lineNumber.begin);
            // STEntry* st_entry=recursiveCheckID(root->symbol_table, root->token_data);
            // printf("ok2\n");
            // //exist nai karta, kuch bhi aur error batana padega
            // if(st_entry==NULL) return TYPE_ERROR;
            // return st_entry->type;
        }
        if(root->token_data->token == NUM) {
            return TYPE_INTEGER;
        }
        if(root->token_data->token == RNUM) {
            return TYPE_REAL;
        }
        if(root->token_data->token == TRUE || root->token_data->token == FALSE) {
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
