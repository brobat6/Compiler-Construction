#include "semanticAnalyzer.h"

Function_Trace* function_trace_stack; // NEVER EMPTY. Contains atleast the driver module.

void push_function_record(char module_name[]) {
    // Adds new function module_name to the current stack. 
    // "function_trace_stack" is updated to point to this new function.
    Function_Trace* new_func = (Function_Trace*)malloc(sizeof(Function_Trace));
    strcpy(new_func->module_name, module_name);
    function_trace_stack->next = new_func;
    new_func->prev = function_trace_stack;
    new_func->next = NULL;
    function_trace_stack = new_func;
}

void pop_function_record() {
    // Removes most recent function call from stack.
    // "function_trace_stack" is updated to point to the previous function.
    Function_Trace* old_func = function_trace_stack->prev;
    free(function_trace_stack);
    function_trace_stack = old_func;
    old_func->next = NULL;
}

bool function_is_recursive(char module_name[]) {
    // Returns true if "module_name" already exists in the function trace stack.
    Function_Trace* temp = function_trace_stack;
    while(temp != NULL) {
        if(strcmp(module_name, temp->module_name) == 0) return true;
        temp = temp->prev;
    }
    return false;
}

// char* get_string_from_type(Type type) {
//     if(type == TYPE_INTEGER) {
//         return "integer";
//     }
//     if(type == TYPE_REAL) {
//         return "real";
//     }
//     if(type == TYPE_BOOLEAN) {
//         return "boolean";
//     }
//     return "undefined";
// }

// NOTE : ADD ASSIGNMENT COUNTER WHEN RETURNING FROM A FUNCTION!!!!!!!!!!!!!!

void functionChecker(Ast_Node* root) {
    if(root == NULL) return;
    if(root->type == 26) {
        /*
        <moduleReuseStmt> -> <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
        Need to check
        1. Module ID has been declared previously.
        2. Parameters returned in <optional> are the same as output parameter list of module ID. 
        3. No recursion
        */
        int line_no = root->child_2->token_data->lineNumber;
        char module_name[21];
        strcpy(module_name, root->child_2->token_data->lexeme); 
        FunctionSTEntry* func = checkFunctionID(module_name);
        if(func == NULL || func->declaration_line_no > line_no) {
            // Function has not been declared previously
            // Note : Can add seperate errors for not declared at all, and for not declared YET.
            Error e;
            e.type = ERROR_FUNCTION_NOT_DECLARED;
            e.line = line_no;
            strcpy(e.module_name, module_name);
            add_error(e);
        } else {
            // Function has been declared previously
            if(strcmp(module_name, root->symbol_table->moduleName) == 0) {
                // Recursion.
                Error e;
                e.type = ERROR_FUNCTION_RECURSIVE;
                e.line = line_no;
                strcpy(e.module_name, module_name);
                strcpy(e.module_name_2, root->symbol_table->moduleName);
                add_error(e);
            } else {
                // No recursion.
                if(line_no < func->first_call_line_no) {
                    func->first_call_line_no = line_no;
                }
                // 1. Check output parameter list.
                if(func->outputParamList->size == 0) {
                    if(root->child_1 != NULL) {
                        // Function is not returning any parameters but is being assigned to something.
                        Error e;
                        e.type = ERROR_FUNCTION_OUTPUT_BEING_ASSIGNED_EXTRA;
                        e.line = line_no;
                        strcpy(e.module_name, module_name);
                        add_error(e);
                    }
                } else {
                    if(root->child_1 == NULL) {
                        // Function is returning parameters but they are not being assigned to anything.
                        Error e;
                        e.type = ERROR_FUNCTION_OUTPUT_NOT_BEING_ASSIGNED;
                        e.line = line_no;
                        strcpy(e.module_name, module_name);
                        add_error(e);
                    } else {
                        ParamListNode* temp1 = func->outputParamList->first;
                        Ast_Node* temp2 = root->child_1->child_1;
                        while(temp1 != NULL && temp2 != NULL) {
                            assert(temp2->type == 33);
                            // Check that the variable temp2 has been declared.
                            STEntry* actual = recursiveCheckID(temp2->symbol_table, temp2->child_1->token_data);
                            if(actual == NULL) {
                                // Variable has not been declared, but is being used.

                                // Error has already been handled by Shivam.

                                // Error e;
                                // e.type = VAR_NOT_INITIALIZED;
                                // e.line = line_no;
                                // strcpy(e.id_name, temp2->child_1->token_data);
                                // add_error(e);
                            } else if(actual->type != temp1->entry->type) {
                                // Variable types do not match!
                                Error e;
                                e.type = ERROR_FUNCTION_OUTPUT_TYPES_DONT_MATCH;
                                e.line = line_no;
                                strcpy(e.id_name, actual->variableName);
                                strcpy(e.id_name_2, temp1->entry->variableName);
                                add_error(e);
                            } else if(actual->isArray) {
                                // Function output parameters can never be an array.
                                Error e;
                                e.type = ERROR_FUNCTION_OUTPUT_ARRAY;
                                e.line = line_no;
                                strcpy(e.id_name, temp1->entry->variableName);
                                add_error(e);
                            } else if(actual->is_for_loop_variable) {
                                // Variable is a for loop variable - assignment not allowed.
                                Error e;
                                e.type = VAR_ASSIGNMENT_TO_FOR_LOOP_VARIABLE;
                                e.line = line_no;
                                strcpy(e.id_name, temp2->child_1->token_data->lexeme);
                                add_error(e);
                            } else {
                                actual->hasBeenAssigned = true;
                            }
                            temp1 = temp1->next;
                            temp2 = temp2->syn_next;
                        }
                        if(temp1 == NULL && temp2 != NULL) {
                            // Function returns LESS parameters than are being assigned.
                            Error e;
                            e.type = ERROR_FUNCTION_OUTPUT_BEING_ASSIGNED_EXTRA;
                            e.line = line_no;
                            strcpy(e.module_name, module_name);
                            add_error(e);
                        }
                        if(temp1 != NULL && temp2 == NULL) {
                            // Function returns MORE parameters than are being assigned.
                            Error e;
                            e.type = ERROR_FUNCTION_OUTPUT_NOT_BEING_ASSIGNED;
                            e.line = line_no;
                            strcpy(e.module_name, module_name);
                            add_error(e);
                        }
                    }
                }

                /*
                2. Check input parameter list.
                Note : In our grammar, things like x[2..20], -y would have been allowed.
                But actually, only variable names must be passed to input parameter list.
                So, I am going to consider only those cases. If needed, we can fix our
                grammar & parser later on.
                */
                assert(func->inputParamList != NULL); // Our grammar won't allow this, but check once!!!
                ParamListNode* temp1 = func->inputParamList->first;
                Ast_Node* temp2 = root->child_3;
                while(temp1 != NULL && temp2 != NULL) {
                    STEntry* actual = recursiveCheckID(temp2->symbol_table, temp2->child_1->child_1->child_1->token_data);
                    // actual ^^ assuming that temp2 can only give ID production (check once). (Will give segfault if assumption is wrong)
                    if(actual == NULL) {
                        // Variable has not been declared, but is being used.

                        // Error is already being handled by Shivam.

                        // Error e;
                        // e.type = VAR_NOT_INITIALIZED;
                        // e.line = line_no;
                        // strcpy(e.id_name, temp2->child_1->child_1->child_1->token_data);
                        // add_error(e);
                    } else {
                        // big_flag : to check if sizes match in case sizes are statically computable
                        bool big_flag = actual->isArray && temp1->entry->isArray && actual->isDynamic.lower == false && actual->isDynamic.upper == false && temp1->entry->isDynamic.lower == false && temp1->entry->isDynamic.upper == false && (actual->range.upper.value - actual->range.lower.value) != (temp1->entry->range.upper.value - temp1->entry->range.lower.value);
                        // printf("%d %d\n", line_no, big_flag);
                        if(actual->type != temp1->entry->type || actual->isArray != temp1->entry->isArray || big_flag) {
                            // Type mismatch.
                            Error e;
                            e.type = ERROR_FUNCTION_INPUT_TYPES_DONT_MATCH;
                            e.line = line_no;
                            strcpy(e.id_name, actual->variableName);
                            strcpy(e.id_name_2, temp1->entry->variableName);
                            add_error(e);
                        } else {
                            // NOTE : UNIMPLEMENTED CODE FOR LOWER/UPPER BOUND CHECKING.
                            // UPD : Implemented above in big_flag.
                        }
                    }
                    temp1 = temp1->next;
                    temp2 = temp2->syn_next;
                }
                if(temp1 == NULL && temp2 != NULL) {
                    // Module takes MORE input parameters than its definition.
                    Error e;
                    e.type = ERROR_FUNCTION_GETS_MORE_INPUT;
                    e.line = line_no;
                    strcpy(e.module_name, module_name);
                    add_error(e);
                }
                if(temp1 != NULL && temp2 == NULL) {
                    // Module takes LESS input parameters than its definition.
                    Error e;
                    e.type = ERROR_FUNCTION_GETS_LESS_INPUT;
                    e.line = line_no;
                    strcpy(e.module_name, module_name);
                    add_error(e);
                }
            }
        }
    }
    if(root->type == 6) {
        /*
        <module> -> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
        Need to check in the output parameter list, that every output parameter has been assigned
        a value atleast once. If not, it is an error.
        */
        int line_no = root->token_data->lineNumber;
        char module_name[21];
        strcpy(module_name, root->child_1->token_data->lexeme); 
        FunctionSTEntry* func = checkFunctionID(module_name);
        if(line_no != func->definition_line_no) return;
        // func can't be NULL as generateST would have populated it.
        ParamListNode* temp = func->outputParamList->first;
        while(temp != NULL) {
            if(temp->entry->hasBeenAssigned == false) {
                Error e;
                e.type = ERROR_FUNCTION_RETURN_PARAMETER_NOT_BEING_ASSIGNED;
                e.line = root->child_4->child_3->token_data->lineNumber;
                strcpy(e.id_name, temp->entry->variableName);
                strcpy(e.module_name, module_name);
                add_error(e);
            }
            temp = temp->next;
        }
    }
    if(root->type == 46) {
        // <conditionalStmt> -> SWITCH BO ID BC START <caseStmts> <default> END

    }
    functionChecker(root->child_1);
    functionChecker(root->child_2);
    functionChecker(root->child_3);
    functionChecker(root->child_4);
    functionChecker(root->child_5);
    functionChecker(root->syn_next);
}

void redundant_call_checker() {
    ht_itr it = ht_iterator(return_function_ST());
    while(ht_next_entry(&it)) {
        FunctionSTEntry* data = it.data;
        if(data->first_call_line_no > data->definition_line_no && data->first_call_line_no != 1000000000 && data->declared_exclusively) {
            // printf("%s %d %d\n", data->moduleName, data->first_call_line_no, data->definition_line_no);
            Error e;
            e.type = ERROR_REDUNDANT_FUNCTION_DECLARATION;
            e.line = data->definition_line_no;
            strcpy(e.module_name, data->moduleName);
            add_error(e);
        }
    }
}

void semanticAnalyzer(Ast_Node* root) {
    /**
     * The main function to be called from driver.c for complete semantic analysis.
    */
    function_trace_stack = (Function_Trace*)malloc(sizeof(Function_Trace));
    strcpy(function_trace_stack->module_name, "driver");
    function_trace_stack->next = NULL;
    function_trace_stack->prev = NULL;
    
    functionChecker(root);
    redundant_call_checker();
}