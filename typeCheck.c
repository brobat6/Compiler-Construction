#include "typeCheck.h"

//Check if the operation is valid, else throw appropriate error.
Type checkType(Token_Info* tk_data, Operator op, Type t1, Type t2) {

    if(t1==TYPE_ERROR || t2==TYPE_ERROR || t1==TYPE_UNDEFINED|| t2==TYPE_UNDEFINED) return TYPE_UNDEFINED;

    Error e;

    if(op==OP_ARITH){
        if(t1==t2 && (t1==TYPE_INTEGER || t1==TYPE_REAL)){
            if(tk_data->token==DIV){
                return TYPE_REAL;
            } else return t1;
        }
        else{
            e.type=ERROR_INCOMPATIBLE_ARITHMETIC_OPERATION;
            e.line=tk_data->lineNumber;
            strcpy(e.id_name, tk_data->lexeme);
            add_error(e);
            return TYPE_ERROR;
        }
    } 
    else if(op==OP_REL){
        if(t1==t2 && (t1==TYPE_INTEGER || t1==TYPE_REAL)) return TYPE_BOOLEAN;
        else{
            e.type=ERROR_INCOMPATIBLE_RELATIONAL_OPERATION;
            e.line=tk_data->lineNumber;
            strcpy(e.id_name, tk_data->lexeme);
            add_error(e);
            return TYPE_ERROR;
        }
    }
    else if(op==OP_LOGIC){
        if(t1==t2 && t1==TYPE_BOOLEAN) {
            return TYPE_BOOLEAN;
        }
        else{
            e.type=ERROR_INCOMPATIBLE_LOGICAL_OPERATION;
            e.line=tk_data->lineNumber;
            strcpy(e.id_name, tk_data->lexeme);
            add_error(e);
            return TYPE_ERROR;
        }
    // } else if(op==OP_ARR){ // Also add cases for Real and Boolean
    //     if(t1==t2 && t1==TYPE_INTEGER) return TYPE_INTEGER;
    //     else{
    //         printf("Expression types of LHS RHS (Arr) dont match\n");
    //         return TYPE_ERROR;
    //     }
    } else if(op==OP_ASSIGN){
        if(t1==t2) return TYPE_UNDEFINED;
        else{
            e.type=ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION;
            e.line=tk_data->lineNumber;
            add_error(e);
            return TYPE_ERROR;
        }
    }
    else{
        e.type=ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION;
        e.line=tk_data->lineNumber;
        add_error(e);
        return TYPE_ERROR;
    }
}

int boundCheck(Token_Info* tk_data, STEntry* st_entry, int index){
    int lower_bound=st_entry->range.lower.value;
    int upper_bound=st_entry->range.upper.value;

    if(index>=lower_bound && index<=upper_bound) return 1;
    else{
        // printf("Line %d: Array %s out of bounds.\n", tk_data->lineNumber, tk_data->lexeme);
        Error e;
        e.type=ERROR_ARRAY_OUT_OF_BOUNDS;
        strcpy(e.id_name, st_entry->variableName);
        e.line=tk_data->lineNumber;
        add_error(e);
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

    if(root == NULL) return TYPE_UNDEFINED;

    // DEF MODULE ID ... 
    // Don't want to check types in input_plist or ret
    if(root->type == 6) {
        return typecheckdfs(root->child_4);
    }

    //Type checking at assignment operation.
    if(root->type==22){
        // 46. <assignmentStmt> -> ID <whichStmt>
        STEntry* st_id = recursiveCheckID(root->child_1->symbol_table, root->child_1->token_data);

        //Checks if entry exists for the ID to which value is being assigned.
        if(st_id==NULL){
            Error e;
            e.type=VAR_NOT_INITIALIZED;
            e.line=root->child_1->token_data->lineNumber;
            strcpy(e.id_name,root->child_1->token_data->lexeme);
            add_error(e);
            
            root->datatype = TYPE_ERROR;
            return TYPE_ERROR;
        }
        st_id->hasBeenAssigned=true;

        //Checkis if assignment is being done to loop variable.
        if(st_id->is_for_loop_variable==true){
            Error e;
            e.type=VAR_ASSIGNMENT_TO_FOR_LOOP_VARIABLE;
            e.line=root->child_1->token_data->lineNumber;
            strcpy(e.id_name,root->child_1->token_data->lexeme);
            add_error(e);

            root->datatype = TYPE_ERROR;
            return TYPE_ERROR;
        }

        Type left_expr=typecheckdfs(root->child_1);

        // printf("%d %d %d %d\n", st_id->isArray, st_id->isDynamic.lower, st_id->isDynamic.upper, root->child_1->type);

        if(st_id->isArray && st_id->isDynamic.lower == false && st_id->isDynamic.upper == false && root->child_2->type == 23) {
            // Check for expressions of the form a := b, where a is a static array.
            Ast_Node* exp = root->child_2->child_1;
            // AnyTerm --> exp->child_1. 
            // ArithExp --> exp->child_1->child_1
            // Term --> exp->child_1->child_1->child_1
            // Factor --> exp->child_1->child_1->child_1->child_1
            if(exp->type != 35 || exp->syn_next != NULL || exp->child_1->child_2 != NULL || exp->child_1->child_1->syn_next != NULL || exp->child_1->child_1->child_1->syn_next != NULL) {
                Error e;
                e.type = ERROR_INCOMPATIBLE_ARRAY_ASSIGNMENT_OPERATION;
                e.line = root->child_1->token_data->lineNumber;
                strcpy(e.id_name, root->child_1->token_data->lexeme);
                add_error(e);
                root->datatype = TYPE_ERROR;
                return TYPE_ERROR;
            }
            Ast_Node* factor = exp->child_1->child_1->child_1->child_1;
            if(factor->child_2 != NULL || factor->type != 40) {
                Error e;
                e.type = ERROR_INCOMPATIBLE_ARRAY_ASSIGNMENT_OPERATION;
                e.line = root->child_1->token_data->lineNumber;
                strcpy(e.id_name, root->child_1->token_data->lexeme);
                add_error(e);
                root->datatype = TYPE_ERROR;
                return TYPE_ERROR;
            }
            STEntry* rhs_id = recursiveCheckID(factor->child_1->symbol_table, factor->child_1->token_data);
            if(rhs_id == NULL) {
                Error e;
                e.type=VAR_NOT_INITIALIZED;
                e.line=factor->child_1->token_data->lineNumber;
                strcpy(e.id_name,factor->child_1->token_data->lexeme);
                add_error(e);
                root->datatype = TYPE_ERROR;
                return TYPE_ERROR;
            }
            if(rhs_id->isArray == false) {
                Error e;
                e.type = ERROR_INCOMPATIBLE_ARRAY_ASSIGNMENT_OPERATION;
                e.line = root->child_1->token_data->lineNumber;
                strcpy(e.id_name, root->child_1->token_data->lexeme);
                add_error(e);
                root->datatype = TYPE_ERROR;
                return TYPE_ERROR;
            }
            if(rhs_id->isDynamic.lower == false && rhs_id->isDynamic.upper == false) {
                int sz_left = st_id->range.upper.value - st_id->range.lower.value;
                int sz_right = rhs_id->range.upper.value - rhs_id->range.lower.value;
                if(sz_left != sz_right) {
                    Error e;
                    e.type = ERROR_UNEQUAL_ARRAY_SIZES;
                    e.line = root->child_1->token_data->lineNumber;
                    strcpy(e.id_name, root->child_1->token_data->lexeme);
                    add_error(e);
                    root->datatype = TYPE_ERROR;
                    return TYPE_ERROR;
                }
            }
            if(rhs_id->type==left_expr){
                root->datatype=left_expr;
                return root->datatype;
            } else{
                if(rhs_id->type == TYPE_ERROR) {
                    root->datatype = TYPE_ERROR;
                    return root->datatype;
                }
                Error e;
                e.type=ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION;
                e.line=factor->child_1->token_data->lineNumber;
                add_error(e);

                root->datatype=TYPE_ERROR;
                return TYPE_ERROR;
            }
        }
        Type right_expr=typecheckdfs(root->child_2);
        // printf("%d, %d\n", left_expr, right_expr);
        //Type check LHS and RHS of assignment.
        root->datatype = checkType(root->child_1->token_data, OP_ASSIGN, left_expr, right_expr);
        return root->datatype;
    }

    //Type checking at unary operations on expressions.
    if(root->type==34){
        Type right_expr=typecheckdfs(root->child_2);

        //If unary operators like '+' or '-' are being assigned, it should be integer or real.
        if(right_expr==TYPE_BOOLEAN){
            Error e;
            e.type=UNARY_ASSIGNMENT_TO_NON_NUM_RNUM;
            e.line=root->child_1->token_data->lineNumber;
            add_error(e);

            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        root->datatype=right_expr;
        return right_expr;
    }
    
    //Type checking rules for right expanding expressions.
    if(root->type==35 || root->type == 38 || root->type==39 || root->type==43 || root->type==44) {
        //Check if it is the highest level of expansion.
        if(root->child_2==NULL) {
            if(root->syn_next == NULL) {
                root->datatype=typecheckdfs(root->child_1);
                return root->datatype;
            }

            root->datatype=typecheckdfs(root->syn_next);
            return root->datatype;
        }

        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr;

        Operator op = token_to_op(root->child_1->token_data->token);

        //Check if it is the lowest level of expansion. Checking at syn_next because child_2 is inherited at syn_next.
        if(root->syn_next == NULL) {
            right_expr = typecheckdfs(root->child_2);
        } else {
            right_expr = typecheckdfs(root->syn_next);
        }

        root->datatype=checkType(root->child_1->token_data, op, left_expr, right_expr);
        return root->datatype;
    }

    //Type Checking for relational expansion.
    if(root->type==36) {
        if(root->child_2==NULL){
            root->datatype=typecheckdfs(root->child_1);
            return root->datatype; // NOTE : DO THIS EVERYWHERE !!!!!!!!!!!!!!!!!!!!!
        } else {
            root->datatype=typecheckdfs(root->child_2);
            return root->datatype;
        }
    }

    // Type checking at array index expansion. Format SIMILAR to type 36.
    if(root->type == 40) {
        if(root->child_2==NULL){
            STEntry* st_id = recursiveCheckID(root->child_1->symbol_table, root->child_1->token_data);
            if(st_id != NULL && st_id->isArray) {
                Error e;
                e.type = ERROR_INCOMPATIBLE_ARRAY_OPERATION;
                e.line = root->child_1->token_data->lineNumber;
                strcpy(e.id_name, root->child_1->token_data->lexeme);
                add_error(e);

                root->datatype = TYPE_ERROR;
                return root->datatype;
            }
            root->datatype=typecheckdfs(root->child_1);
            return root->datatype; // NOTE : DO THIS EVERYWHERE !!!!!!!!!!!!!!!!!!!!!
        } else {
            root->datatype=typecheckdfs(root->child_2);
            return root->datatype;
        }
    }

    //Type checking for relational operations.
    if(root->type==37){
        Type left_expr=typecheckdfs(root->inh_1);
        Type right_expr=typecheckdfs(root->child_2);

        Operator op=token_to_op(root->child_1->token_data->token);

        root->datatype=checkType(root->inh_1->token_data, op, left_expr, right_expr);
        return root->datatype;
    }

    // <moduleReuseStmt> -> <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
    // Here, the only issue is that "USE MODULE ID" was giving an error wrongly, because 
    // the code checks for all ID, whether they have been put in symbol table or not.
    // But MODULE ID will actually be in function symbol table, and it is being 
    // check in the semanticAnalyzer code. So we need to ignore this.
    if(root->type == 26) {
        typecheckdfs(root->child_1);
        typecheckdfs(root->child_3);
        root->datatype = TYPE_UNDEFINED;
        return TYPE_UNDEFINED;
    }

    //Type and statically bound check for assignment on array elements.
    if(root->type == 31 || root->type == 24) {
        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr = typecheckdfs(root->child_1);

        //We try to find if the array exists else we throw an error when left_expr is called.
        STEntry* st_entry=recursiveCheckID(root->symbol_table, root->inh_1->token_data);
        if(left_expr!=TYPE_INTEGER && left_expr!=TYPE_REAL){
            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        //If the ID is not of array type we throw an error.
        if(!st_entry->isArray) {
            Error e;
            e.type= VAR_NOT_OF_TYPE_ARRAY;
            e.line=root->inh_1->token_data->lineNumber;
            strcpy(e.id_name, root->inh_1->token_data->lexeme);
            add_error(e);

            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        if(root->type == 24){
            //Then we check if type of LHS and RHS are the same.
            Type tt = typecheckdfs(root->child_2);
            if(tt == TYPE_ERROR || tt == TYPE_UNDEFINED) {
                root->datatype = TYPE_ERROR;
                return root->datatype;
            }
            if(st_entry->type!=tt){
                Error e;
                e.type=ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION;
                e.line=root->inh_1->token_data->lineNumber;
                strcpy(e.id_name, root->inh_1->token_data->lexeme);
                add_error(e);

                root->datatype=TYPE_ERROR;
                return root->datatype;
            }
        }

        //We check if array is not dynamic, because dynamic bound checking is done at code generation and here we just static bound check.
        if(!st_entry->isDynamic.lower && !st_entry->isDynamic.upper){
            //Here we store the static index.
            int arr_index;

            //This handles the case when '+' precedes the NUM-type index.
            if(root->child_1->type==41 && root->child_1->child_1->token_data->token==NUM){
                arr_index = root->child_1->child_1->token_data->value.num;
            } 
            
            //This handles the case when '-' precedes the NUM-type index.
            else if(root->child_1->type==42 && root->child_1->child_1->token_data->token==NUM){
                arr_index = root->child_1->child_1->token_data->value.num;
                arr_index*=-1;
            } 
            
            //This handles if there is no prefix, then we have to type check for ID, NUM or expressions as index.
            else if(root->child_1->type==43){

                //This checks for non-expression NUM type index.
                if(root->child_1->syn_next==NULL && root->child_1->child_1->syn_next==NULL && root->child_1->child_1->child_1->token_data->token==NUM){
                    arr_index = root->child_1->child_1->child_1->token_data->value.num;
                }

                //If we have an expression, we get its type by calling type check on the expression itself.
                else{
                    Type index_expr = typecheckdfs(root->child_1);

                    //Since the epression is of integer type, we have type checked and bound check for expressions is dynamic.
                    if(index_expr==TYPE_INTEGER){
                        root->datatype=left_expr;
                        return left_expr;
                    }

                    //We return a error if index type is not integer.
                    else{
                        Error e;
                        e.type=NON_INTEGER_ARRAY_INDEX;
                        e.line=root->inh_1->token_data->lineNumber;
                        strcpy(e.id_name, root->inh_1->token_data->lexeme);
                        add_error(e);

                        root->datatype=TYPE_ERROR;
                        return TYPE_ERROR;
                    }
                }
            } 
            
            //This type checks for ID type index and returns without bound checking as it is done dynamically for ID.
            else if(root->child_1->child_1->token_data->token!=NUM){

                //We check if the ID is declared or not first.
                STEntry* st_id = recursiveCheckID(root->child_1->child_1->symbol_table, root->child_1->child_1->token_data);
                if(st_entry==NULL){
                    Error e;
                    e.type=VAR_NOT_INITIALIZED;
                    e.line=root->inh_1->token_data->lineNumber;
                    strcpy(e.id_name,root->inh_1->token_data->lexeme);
                    add_error(e);
                    
                    root->datatype = TYPE_ERROR;
                    return TYPE_ERROR;
                }

                //Checks for integer type of index, else throws an error.
                if(st_id->type==TYPE_INTEGER){
                    root->datatype=left_expr;
                    return left_expr;
                }

                else{
                    Error e;
                    e.type=NON_INTEGER_ARRAY_INDEX;
                    e.line=root->inh_1->token_data->lineNumber;
                    strcpy(e.id_name, root->inh_1->token_data->lexeme);
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }
            }   

            //Here, we perform bound checking and error is reported inside the boundCheck function.
            if(boundCheck(root->inh_1->token_data, st_entry, arr_index)){
                root->datatype=left_expr;
                return left_expr;
            }

            else{
                root->datatype=TYPE_ERROR;
                return left_expr;
            }
        }
    }

    //Type and statically bound check
    if(root->type==21){
        Type left_expr = typecheckdfs(root->inh_1);
        Type right_expr = typecheckdfs(root->child_1);

         //We try to find if the array exists else we throw an error when left_expr is called.
        STEntry* st_entry=recursiveCheckID(root->symbol_table, root->inh_1->token_data);
        if(left_expr!=TYPE_INTEGER && left_expr!=TYPE_REAL){
            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        //If the ID is not of array type we throw an error.
        if(!st_entry->isArray) {
            Error e;
            e.type= VAR_NOT_OF_TYPE_ARRAY;
            e.line=root->inh_1->token_data->lineNumber;
            strcpy(e.id_name, root->inh_1->token_data->lexeme);
            add_error(e);

            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        //We check if array is not dynamic, because dynamic bound checking is done at code generation and here we just static bound check.
        if(!st_entry->isDynamic.lower && !st_entry->isDynamic.upper){
            //Here we store the static index.
            int arr_index;

            //This checks if the associated index has no '+' or '-' sign before it and implemets for those cases.
            if(root->child_1->child_1==NULL){

                //If the index is a variable we see if the type is integer, else we throw an error.
                if(root->child_1->child_2->token_data->token!=NUM){

                    //Here we check if the variable has been declared.
                    STEntry* st_id = recursiveCheckID(root->child_1->child_2->symbol_table, root->child_1->child_2->token_data);
                    if(st_entry==NULL){
                        Error e;
                        e.type=VAR_NOT_INITIALIZED;
                        e.line=root->child_1->child_2->token_data->lineNumber;
                        strcpy(e.id_name,root->child_1->child_2->token_data->lexeme);
                        add_error(e);
                        
                        root->datatype = TYPE_ERROR;
                        return TYPE_ERROR;
                    }

                    //If it is integer our type checking is done and we return nothing, else we throw an error.
                    if(st_id->type==TYPE_INTEGER){
                        root->datatype=TYPE_UNDEFINED;
                        return TYPE_UNDEFINED;
                    }
                    else{
                        Error e;
                        e.type=NON_INTEGER_ARRAY_INDEX;
                        e.line=root->child_1->child_2->token_data->lineNumber;
                        strcpy(e.id_name, root->child_1->child_2->token_data->lexeme);
                        add_error(e);

                        root->datatype=TYPE_ERROR;
                        return TYPE_ERROR;
                    }
                }
            } 
            
            //This handles the case when '+' precedes the NUM-type index.
            if(root->child_1->child_1->token_data->token==PLUS && root->child_1->child_2->token_data->token==NUM){
                arr_index = root->child_1->child_2->token_data->value.num;
            }

            //This handles the case when '-' precedes the NUM-type index.
            else if(root->child_1->child_1->token_data->token==MINUS && root->child_1->child_2->token_data->token==NUM){
                arr_index = root->child_1->child_2->token_data->value.num;
                arr_index*=-1;
            }

            //This handles the case when the index is a sign-less ID.
            else if(root->child_1->child_2->token_data->token==ID){

                //Here we check if the variable has been declared.
                STEntry* st_id = recursiveCheckID(root->child_1->child_2->symbol_table, root->child_1->child_2->token_data);
                if(st_entry==NULL){
                    Error e;
                    e.type=VAR_NOT_INITIALIZED;
                    e.line=root->child_1->child_2->token_data->lineNumber;
                    strcpy(e.id_name,root->child_1->child_2->token_data->lexeme);
                    add_error(e);
                        
                    root->datatype = TYPE_ERROR;
                    return TYPE_ERROR;
                }

                //If it is integer our type checking is done and we return nothing, else we throw an error.
                if(st_id->type==TYPE_INTEGER){
                    root->datatype=TYPE_UNDEFINED;
                    return TYPE_UNDEFINED;
                }
                else{
                    Error e;
                    e.type=NON_INTEGER_ARRAY_INDEX;
                    e.line=root->child_1->child_2->token_data->lineNumber;
                    strcpy(e.id_name, root->child_1->child_2->token_data->lexeme);
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }
            }

            //Here, we perform bound checking and error is reported inside the boundCheck function.
            if(boundCheck(root->inh_1->token_data, st_entry, arr_index)){
                root->datatype=TYPE_UNDEFINED;
                return TYPE_UNDEFINED;
            }

            else{
                root->datatype=TYPE_ERROR;
                return TYPE_ERROR;
            }
        }
    }

    //Type checking for switch-case conditional statements
    if(root->type==46){

        //First, we get the type of switch variable and depending on its type we have different cases shown below.
        Type left_expr=typecheckdfs(root->child_1);

        //Check if switch-case is of type integer or boolean, else throw an error.
        if(left_expr!=TYPE_INTEGER && left_expr!=TYPE_BOOLEAN){
            Error e;
            e.type=INVALID_SWITCH_TYPE_FOR_CASES;
            e.line=root->child_1->token_data->lineNumber;
            add_error(e);
            
            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        typecheckdfs(root->child_3);
        typecheckdfs(root->child_4);

        //Check that boolean switch-case does not have a default type, else throw an error.
        if(left_expr==TYPE_BOOLEAN && root->child_4!=NULL){
            Error e;
            e.type=DEFAULT_CASE_IN_BOOLEAN_SWITCH;
            e.line=root->child_1->token_data->lineNumber;
            add_error(e);
            
            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }

        //Check that integer switch-case has a default type, else throw an error.
        if(left_expr==TYPE_INTEGER && root->child_4==NULL){
            Error e;
            e.type=DEFAULT_CASE_IN_INTEGER_SWITCH;
            e.line=root->child_5->token_data->lineNumber;
            add_error(e);
            
            root->datatype=TYPE_ERROR;
            return TYPE_ERROR;
        }
        
        //Here we recurse on all the case statements belonging to the switch and type check on them. The flag is used as a loop variable which
        //gets set when we reach the last case/bottom of recursion to break the loop.
        Ast_Node* node =root->child_3;
        int flag=0;

        if(left_expr==TYPE_INTEGER){
            while(flag==0){

                //For integer case we have to check if case type is integer and report errors for all cases that are not of integer type.
                if(typecheckdfs(node->child_1)!=TYPE_INTEGER) {
                    //Assign error to the case statement type.
                    node->child_1->type=TYPE_ERROR;

                    Error e;
                    e.type=CASE_NOT_INTEGER_TYPE_FOR_INTEGER_SWITCH;
                    e.line=node->child_1->token_data->lineNumber;
                    strcpy(e.id_name, node->child_1->token_data->lexeme);
                    add_error(e);
                }

                //Change node to check the next case statement.
                node=node->child_3;
                if(node==NULL) flag++;
            }

            //Return undefined because we do not have to type check above.
            root->datatype=TYPE_UNDEFINED;
            return TYPE_UNDEFINED;
        } 
        
        if(left_expr==TYPE_BOOLEAN){

            //'fl' is a set fo two flags based on binary bit-wise representation. fl can take 4 values: 0, 1, 2, 3. The 1st bit/MSB checks if a
            //'case true' occurs and 2nd bit/LSB checks if a 'case false' occurs. This helps report error in case either case do not occur.
            int fl=0;

            while(flag==0){


                //For boolean case we have to check if case type is boolean and report errors for all cases that are not of boolean type.
                if(typecheckdfs(node->child_1)!=TYPE_BOOLEAN) {
                    //Assign error to the case statement type.
                    node->child_1->type=TYPE_ERROR;

                    Error e;
                    e.type=CASE_NOT_BOOLEAN_TYPE_FOR_BOOLEAN_SWITCH;
                    e.line=node->child_1->token_data->lineNumber;
                    strcpy(e.id_name, node->child_1->token_data->lexeme);
                    add_error(e);
                }

                else{
                    if(node->child_1->token_data->token==TRUE){
                        //Set MSB of 'fl' if not set, if set we ignore.
                        if(fl==0 || fl==1) fl+=2;
                    } else if(node->child_1->token_data->token==FALSE){
                        //Set LSB of 'fl' if not set, if set we ignore.
                        if(fl==0 || fl==2) fl++;
                    }
                }

                //Change node to check the next case statement.
                node=node->child_3;
                if(node==NULL) flag++;
            }

            //If both bits are set, case true and false both have occurred and hence we have to report no errors.
            if(fl==3){
                root->datatype=TYPE_UNDEFINED;
                return TYPE_UNDEFINED;
            }

            //Else we have to report appropriate error as to which case true and/or false have not occurred.
            else{
                if(fl==0){
                    Error e;
                    e.type=MISSING_TRUE_FALSE_CASES;
                    e.line=root->child_5->token_data->lineNumber;
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }
                if(fl==1){
                    Error e;
                    e.type=MISSING_TRUE_CASES;
                    e.line=root->child_5->token_data->lineNumber;
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }
                if(fl==2){
                    Error e;
                    e.type=MISSING_FALSE_CASES;
                    e.line=root->child_5->token_data->lineNumber;
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }

                //In some case (which won't happen) flag value becomes soething outside our utility, we throw an unexpected error.
                else{
                    Error e;
                    e.type=UNEXPECTED_FLAG_VALUE;
                    e.line=root->child_5->token_data->lineNumber;
                    add_error(e);

                    root->datatype=TYPE_ERROR;
                    return TYPE_ERROR;
                }
            }
        }
        
    }

    //Return type of case to the parent function where it is checked if it matches switch type.
    if(root->type==47){
        root->datatype=typecheckdfs(root->child_1);
        typecheckdfs(root->child_2);
        typecheckdfs(root->child_3);
        return root->datatype;
    }

    //Type checking ID and other terminals and returning accurate types for type checking.
    if(root->type==0){
        
        //If terminal is ID we have to check if it is declared and in scope or not.
        if(root->token_data->token == ID) {
            STEntry* st_entry=recursiveCheckID(root->symbol_table, root->token_data);
            if(st_entry==NULL){
                Error e;
                e.type=VAR_NOT_INITIALIZED;
                e.line=root->token_data->lineNumber;
                strcpy(e.id_name,root->token_data->lexeme);
                add_error(e);
                
                root->datatype = TYPE_ERROR;
                return TYPE_ERROR;
            }

            root->datatype=st_entry->type;
            return st_entry->type;
        }

        if(root->token_data->token == NUM) {
            root->datatype=TYPE_INTEGER;
            return TYPE_INTEGER;
        }

        if(root->token_data->token == RNUM) {
            root->datatype=TYPE_REAL;
            return TYPE_REAL;
        }

        if(root->token_data->token == TRUE || root->token_data->token == FALSE) {
            root->datatype=TYPE_BOOLEAN;
            return TYPE_BOOLEAN;
        }

        else{
            // Error e;
            // e.type=UNKNOWN_TERMINAL_TYPE;
            // e.line=root->token_data->lineNumber;
            // strcpy(e.id_name,root->token_data->lexeme);
            // add_error(e);
                
            root->datatype = TYPE_ERROR;
            return TYPE_ERROR;
        }
    }

    if(root->type == 23) {
        root->datatype = typecheckdfs(root->child_1);
        return root->datatype;
    }

    //For other rules, we do not need to type/bound/scope check. This code segment is just to traverse the tree.
    Type t;
    if(root->child_1!=NULL) t=typecheckdfs(root->child_1);
    if(root->child_2!=NULL) t=typecheckdfs(root->child_2);
    if(root->child_3!=NULL) t=typecheckdfs(root->child_3);
    if(root->child_4!=NULL) t=typecheckdfs(root->child_4);
    if(root->child_5!=NULL) t=typecheckdfs(root->child_5);
    if(root->syn_next!=NULL) t=typecheckdfs(root->syn_next);

    root->datatype=TYPE_UNDEFINED;
    return TYPE_UNDEFINED;
}
