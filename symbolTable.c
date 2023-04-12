#include "symbolTable.h"

#define DATASEGMENTOFFSET 1000
int curOffset=DATASEGMENTOFFSET;
STTreeNode* curSTTreeNode=NULL;
STTreeNode* globalSTRoot=NULL; // Root of symbol table
STEntry* tempEntry;
ht* functionST; // Function symbol table

STTreeNode* createSTTreeNode(){             ///can use current AST node for data;
    STTreeNode* newNode = (STTreeNode*)malloc(sizeof(STTreeNode));
    newNode->hashTable=init_ht();
    newNode->offset=curOffset;
    newNode->nodeWidth = 0;
    newNode->parent=NULL;
    newNode->leftMostChild=NULL;
    newNode->rightMostChild=NULL;
    newNode->sibling=NULL;
    return newNode;
}

void addSTChild(STTreeNode *parent,STTreeNode *child){
    if(parent->leftMostChild==NULL){
        parent->leftMostChild=child;
        parent->rightMostChild=child;
    }
    else{
        parent->rightMostChild->sibling=child;
        parent->rightMostChild=child;
    }
    child->parent=parent;
    child->nestingLevel=parent->nestingLevel+1;
}

STEntry* createSTEntry(STEntry* prevEntry) {
    STEntry* newNode = NULL;
    if(prevEntry != NULL) {
        newNode = (STEntry*)malloc(sizeof(STEntry));
        strcpy(newNode->variableName, prevEntry->variableName);
        newNode->type = prevEntry->type;
        newNode->isArray = prevEntry->isArray;
        newNode->isDynamic = prevEntry->isDynamic;
        newNode->range = prevEntry->range;
        newNode->declarationLineNumber = prevEntry->declarationLineNumber;
        newNode->width = prevEntry->width;
        newNode->offset = prevEntry->offset;
        newNode->is_for_loop_variable = prevEntry->is_for_loop_variable;
        newNode->isOutputParameter = false;
        newNode->hasBeenAssigned = false;
    }
    return newNode;
}

STEntry* checkID(STTreeNode* node,char lexeme[]){      //confirm input parameter
    return ht_fetch(node->hashTable,lexeme);
}

FunctionSTEntry* checkFunctionID(char lexeme[]) {
    return ht_fetch(functionST, lexeme);
}

STEntry* recursiveCheckID(STTreeNode* node,Token_Info* t) {
    /**
     * Successively goes in outward scopes until it finds a scope which has 
     * token t declared BEFORE the line number of t. It does NOT return any
     * node which is declared AFTER t. If there is no valid node, it returns
     * NULL.
     * Assumption : declaration of t and call of t are never in the same line 
     * (solving for this case requires a lot of effort).
    */
    if(node == NULL) return NULL;
    STEntry* check = checkID(node, t->lexeme);
    if(check == NULL || check->declarationLineNumber > t->lineNumber) {
        return recursiveCheckID(node->parent, t);
    }
    return check;
}

ParamList* initialize_parameter_list() {
    ParamList* pl = (ParamList*)malloc(sizeof(ParamList));
    pl->size = 0;
    pl->first = NULL;
    pl->last = NULL;
    return pl;
}

void add_to_parameter_list(ParamList* pl, STEntry* st) {
    ParamListNode* pln = (ParamListNode*)malloc(sizeof(ParamListNode));
    pln->entry = st;
    pln->next = NULL;
    pl->size++;
    if(pl->first == NULL) {
        pl->first = pln;
        pl->last = pln;
    } else {
        assert(pl->last != NULL);
        pl->last->next = pln;
        pl->last = pln;
    }
}

Type get_entry_type(enum Token tokenType) {
    if(tokenType == NUM) {
        return TYPE_INTEGER;
    } else if(tokenType == RNUM) {
        return REAL;
    } else if(tokenType == BOOLEAN) {
        return BOOLEAN;
    } else {
        return TYPE_UNDEFINED;
    }
}

int get_entry_width(Type type) {
    if(type == TYPE_INTEGER) {
        return 2;
    } else if(type == TYPE_REAL) {
        return 4;
    } else if(type == TYPE_BOOLEAN) {
        return 1;
    }
    return 0;
}

void populate_temporary_function_entry(Ast_Node* temp) {
    /***
     * Populate the global variable tempEntry with required <dataType> information, 
     * to be entered into symbol table later.
    */
    assert(temp->type == 10 || temp->type == 12); // <dataType> or <type>
    tempEntry->isOutputParameter = false;
    tempEntry->is_for_loop_variable = false;
    if(temp->child_2 == NULL) {
        enum Token tokenType = temp->child_1->token_data->token;
        if(tokenType == INTEGER) {
            tempEntry->type = TYPE_INTEGER;
        } else if(tokenType == REAL) {
            tempEntry->type = TYPE_REAL;
        } else if(tokenType == BOOLEAN) {
            tempEntry->type = TYPE_BOOLEAN;
        } else {
            tempEntry->type = TYPE_UNDEFINED; // this should never happen.
        }
        tempEntry->isArray = false;
    } else {
        enum Token tokenType = temp->child_2->child_1->token_data->token;
        if(tokenType == INTEGER) {
            tempEntry->type = TYPE_INTEGER;
        } else if(tokenType == REAL) {
            tempEntry->type = TYPE_REAL;
        } else if(tokenType == BOOLEAN) {
            tempEntry->type = TYPE_BOOLEAN;
        } else {
            tempEntry->type = TYPE_UNDEFINED; // this should never happen.
        }
        tempEntry->isArray = true;
        temp = temp->child_1;

        // We are at <range_arrays> now
        assert(temp->type == 11);

        // Lower range
        int temp_sign = 1;
        if(temp->child_1->child_1 != NULL && temp->child_1->child_1->token_data->token == MINUS) {
            temp_sign = -1;
        }
        if(temp->child_1->child_2->token_data->token == NUM) {
            tempEntry->isDynamic.lower = false;
            tempEntry->range.lower.value = temp->child_1->child_2->token_data->value.num * temp_sign;
        } else {
            tempEntry->isDynamic.lower = true;
            strcpy(tempEntry->range.lower.lexeme, temp->child_1->child_2->token_data->lexeme);
            tempEntry->range.lower_sign = temp_sign;
        }
        // Upper range
        temp_sign = 1;
        if(temp->child_2->child_1 != NULL && temp->child_2->child_1->token_data->token == MINUS) {
            temp_sign = -1;
        }
        if(temp->child_2->child_2->token_data->token == NUM) {
            tempEntry->isDynamic.upper = false;
            tempEntry->range.upper.value = temp->child_2->child_2->token_data->value.num * temp_sign;
        } else {
            tempEntry->isDynamic.upper = true;
            strcpy(tempEntry->range.upper.lexeme, temp->child_2->child_2->token_data->lexeme);
            tempEntry->range.upper_sign = temp_sign;
        }
    }
    tempEntry->width = get_entry_width(tempEntry->type);
    if(tempEntry->isArray == true) {
        if(tempEntry->isDynamic.lower == false && tempEntry->isDynamic.upper == false) {
            tempEntry->width *= (tempEntry->range.upper.value - tempEntry->range.lower.value + 1);
            tempEntry->width++; // Because apparently, array name also takes 1 byte.
        } else {
            tempEntry->width = 0; // NEED TO CLARIFY THIS. DYNAMIC ARRAY!!!!! WIDTH, OFFSET HOW???
        }
    }
}

void throw_function_already_exists_error(char module_name[], int line_no) {
    printf("function %s already exists, declared at line %d!\n", module_name, line_no);
}

void throw_already_exists_error(char module_name[], int line_no) {
    printf("Error - Line %d: identifier %s already exists\n", line_no, module_name);
}

void throw_function_not_declared_error(char module_name[], int line_no) {
    printf("Function %s is being called at line %d, but it has not been declared!\n", module_name, line_no);
}

void throw_function_redefinition_error(char module_name[], int line_no) {
    printf("Function %s being redefined at %d, has been defined previously.\n", module_name, line_no);
}

void generateST(STTreeNode* currSTNode, Ast_Node* root) {

    if(root == NULL) return;
    assert(currSTNode != NULL);

    root->symbol_table = currSTNode;

    if(root->type == 3) {
        // module declaration
        FunctionSTEntry* func = (FunctionSTEntry*)malloc(sizeof(FunctionSTEntry));
        strcpy(func->moduleName, root->child_1->token_data->lexeme);
        func->defined = false;
        func->declaration_line_no = root->token_data->lineNumber;
        if(checkFunctionID(func->moduleName) != NULL) {
            // ERROR!!!!! 
            throw_function_already_exists_error(func->moduleName, func->declaration_line_no);
        } else {
            ht_store(functionST, func->moduleName, func);
        }
        generateST(currSTNode, root->child_1);
        return;
    }
    if(root->type == 5) {
        // driver module
        // 1. ID ST Table
        int start_line_no = root->token_data->lineNumber; // DRIVERDEF
        int end_line_no = root->child_1->child_3->token_data->lineNumber; // END
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, "driver");
        childSTNode->offset = curOffset;
        // 2. Function ST Table
        FunctionSTEntry* functionEntry = (FunctionSTEntry*)malloc(sizeof(FunctionSTEntry));
        strcpy(functionEntry->moduleName, "driver");
        functionEntry->defined = true;
        functionEntry->declaration_line_no = start_line_no;
        if(checkFunctionID(functionEntry->moduleName) != NULL) {
            // ERROR!!!!! 
            throw_function_already_exists_error(functionEntry->moduleName, functionEntry->declaration_line_no);
        } else {
            ht_store(functionST, functionEntry->moduleName, functionEntry);
        }
        // 3. Call recursion downward.
        generateST(childSTNode, root->child_1);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        functionEntry->function_width = childSTNode->nodeWidth;
        return;
    }
    if(root->type == 6) {
        // module definition

        /**
         * 1.
         * Check if the function has been declared previously.
         * If yes, check if the function has been defined previously. If yes here too, throw an error.
         * Else, define the function.
         * 
         * If the function has not been declared previously, declare as well as define the function right now. 
        */

        FunctionSTEntry* func = checkFunctionID(root->child_1->token_data->lexeme);
        if(func == NULL) {
            func = (FunctionSTEntry*)malloc(sizeof(FunctionSTEntry));
            strcpy(func->moduleName, root->child_1->token_data->lexeme);
            func->defined = true;
            func->declaration_line_no = root->token_data->lineNumber;
            ht_store(functionST, func->moduleName, func);
        } else if(func->defined == true) {
            throw_function_redefinition_error(func->moduleName, root->token_data->lineNumber);
        } else {
            func->defined = true;
        }

        // 1. Check that the function has been declared. [OLD CODE]
        // FunctionSTEntry* func = checkFunctionID(root->child_1->token_data->lexeme);
        // if(func == NULL) {
        //     throw_function_not_declared_error(root->child_1->token_data->lexeme, root->token_data->lineNumber);
        //     return;
        // }
        // func->defined = true;

        // 2. Create child symbol table

        // 2A. Create input parameter symbol table
        int start_line_no = root->token_data->lineNumber; // DEF
        int end_line_no = root->child_4->child_3->token_data->lineNumber; // <moduleDef> ---> END
        STTreeNode* elder_child_st_node = createSTTreeNode();
        addSTChild(currSTNode, elder_child_st_node);
        elder_child_st_node->nestingLevel = currSTNode->nestingLevel;
        (elder_child_st_node->lineNumber).begin = start_line_no;
        (elder_child_st_node->lineNumber).end = end_line_no;
        strcpy(elder_child_st_node->moduleName, root->child_1->token_data->lexeme);
        elder_child_st_node->offset = curOffset;

        // 2B. Create local symbol table
        start_line_no = root->child_4->child_1->token_data->lineNumber; // <moduleDef> ---> START
        STTreeNode* younger_child_st_node = createSTTreeNode();
        addSTChild(elder_child_st_node, younger_child_st_node);
        (younger_child_st_node->lineNumber).begin = start_line_no;
        (younger_child_st_node->lineNumber).end = end_line_no;
        strcpy(younger_child_st_node->moduleName, root->child_1->token_data->lexeme);

        // 3. Populate input_plist and output_plist
        func->inputParamList = initialize_parameter_list();
        func->outputParamList = initialize_parameter_list();

        /**
         * Input/output parameter specifications by ma'am:
         * Local variables can shadow input parameters but NOT output parameters
         * The nesting level of both input and output parameters must be 0
         * Two input parameters with same ID but in seperate modules are allowed.
         * 
         * Solution implemented for the input/output parameter specifications:
         * Input parameter lists are present in a separate symbol table which 
         * is the parent of the local symbol table for the module. 
         * Output parameter lists are present in the local symbol table, but 
         * have the isOutputParameter flag set to TRUE, since their nesting level 
         * must be -1 that of the local symbol table.
        */

        Ast_Node* temp = root->child_2;
        while(temp != NULL) {
            populate_temporary_function_entry(temp->child_2);
            STEntry* entry = createSTEntry(tempEntry);
            strcpy(entry->variableName, temp->child_1->token_data->lexeme);
            entry->declarationLineNumber = temp->child_1->token_data->lineNumber;
            entry->offset = curOffset;
            curOffset += entry->width;
            // Currently adding input/output lists in CHILD symbol table.
            if(checkID(elder_child_st_node, entry->variableName) != NULL) {
                throw_already_exists_error(entry->variableName, entry->declarationLineNumber);
            } else {
                ht_store(elder_child_st_node->hashTable, entry->variableName, entry);
                add_to_parameter_list(func->inputParamList, entry);
            }
            temp = temp->syn_next;
        }
        temp = root->child_3;
        if(temp != NULL) temp = temp->child_1;

        younger_child_st_node->offset = curOffset;

        while(temp != NULL) {
            populate_temporary_function_entry(temp->child_2);
            STEntry* entry = createSTEntry(tempEntry);
            entry->isOutputParameter = true;
            strcpy(entry->variableName, temp->child_1->token_data->lexeme);
            entry->declarationLineNumber = temp->child_1->token_data->lineNumber;
            entry->offset = curOffset;
            curOffset += entry->width;
            // Currently adding input/output lists in CHILD symbol table.
            // Change if necessary !!!!!!
            if(checkID(younger_child_st_node, entry->variableName) != NULL) {
                throw_already_exists_error(entry->variableName, entry->declarationLineNumber);
            } else {
                ht_store(younger_child_st_node->hashTable, entry->variableName, entry);
                add_to_parameter_list(func->outputParamList, entry);
            }
            temp = temp->syn_next;
        }
        generateST(currSTNode, root->child_1);
        generateST(elder_child_st_node, root->child_2);
        generateST(younger_child_st_node, root->child_3);
        generateST(younger_child_st_node, root->child_4);
        elder_child_st_node->nodeWidth = (curOffset - elder_child_st_node->offset);
        younger_child_st_node->nodeWidth = (curOffset - younger_child_st_node->offset);
        func->function_width = elder_child_st_node->nodeWidth;
        return;
    }
    if(root->type == 45) {
        // Declare statement.
        populate_temporary_function_entry(root->child_1);
        Ast_Node* temp = root->child_2;
        assert(temp != NULL); // i.e., idList is not empty.
        while(temp != NULL) {
            STEntry* newEntry = createSTEntry(tempEntry);
            strcpy(newEntry->variableName, temp->child_1->token_data->lexeme);
            newEntry->declarationLineNumber = temp->child_1->token_data->lineNumber; 
            newEntry->offset = curOffset;
            curOffset += newEntry->width;
            if(checkID(currSTNode, temp->child_1->token_data->lexeme) != NULL) {
                // ERROR!!!!! 
                throw_already_exists_error(newEntry->variableName, temp->child_1->token_data->lineNumber);
            } else {
                ht_store(currSTNode->hashTable, newEntry->variableName, newEntry);
            }
            temp = temp->syn_next;
        }
        generateST(currSTNode, root->child_1);
        generateST(currSTNode, root->child_2);
        return;
    }
    if(root->type == 46) {
        // Conditional statement (SWITCH)
        int start_line_no = root->child_1->token_data->lineNumber;
        int end_line_no = root->child_5->token_data->lineNumber;
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, currSTNode->moduleName);
        childSTNode->offset = curOffset;
        generateST(currSTNode, root->child_1);
        generateST(childSTNode, root->child_2);
        generateST(childSTNode, root->child_3);
        generateST(childSTNode, root->child_4);
        generateST(childSTNode, root->child_5);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    if(root->type == 48) {
        // FOR

        // 1. Create new symbol table
        int start_line_no = root->token_data->lineNumber; // FOR
        int end_line_no = root->child_5->token_data->lineNumber; // END
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, currSTNode->moduleName);
        childSTNode->offset = curOffset;

        // 2. Add entry of ID in child table
        STEntry* id_entry = createSTEntry(tempEntry);
        id_entry->declarationLineNumber = start_line_no;
        strcpy(id_entry->variableName, root->child_1->token_data->lexeme);
        id_entry->is_for_loop_variable = true;
        id_entry->type = TYPE_INTEGER;
        id_entry->isArray = false;
        id_entry->width = 2;
        id_entry->offset = curOffset;
        curOffset += 2;
        ht_store(childSTNode->hashTable, id_entry->variableName, id_entry);
        
        generateST(currSTNode, root->child_1);
        // 3. Call with child symbol table inside for loop
        generateST(currSTNode, root->child_2); /*
        1. Doing this for type checking etc later, as symbol table needs to be populated
        2. What happens in case we have FOR X IN RANGE [X...3], i.e. range has same identifier declared previously?
        Current ans. It would not give error, the X in range would be taken from outer scope. 
        */
       generateST(currSTNode, root->child_3);
        generateST(childSTNode, root->child_4);
        generateST(currSTNode, root->child_5);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    if(root->type == 49) {
        // WHILE statement.
        int start_line_no = root->token_data->lineNumber;
        int end_line_no = root->child_4->token_data->lineNumber;
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, currSTNode->moduleName);
        childSTNode->offset = curOffset;
        generateST(currSTNode, root->child_1); // Just done so that ast_node->symbol_table elements are populated (for easy type checking later)
        generateST(currSTNode, root->child_2);
        generateST(childSTNode, root->child_3);
        generateST(currSTNode, root->child_4);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    
    generateST(currSTNode, root->child_1);
    generateST(currSTNode, root->child_2);
    generateST(currSTNode, root->child_3);
    generateST(currSTNode, root->child_4);
    generateST(currSTNode, root->child_5);
    generateST(currSTNode, root->syn_next);
}

STTreeNode* generateSymbolTable(Ast_Node* ASTRoot){      ////confirm input parameter; INCOMPLETE
    STTreeNode* STRoot = createSTTreeNode();
    globalSTRoot=STRoot;
    STRoot->nestingLevel = 0;
    tempEntry = (STEntry*)malloc(sizeof(STEntry));
    strcpy(STRoot->moduleName,"ROOT");
    functionST = init_ht();

    generateST(STRoot, ASTRoot);

    return STRoot;
}

void recursive_print_symbol_table(STTreeNode* root, FILE* fp) {
    ht_itr it = ht_iterator(root->hashTable);
    while(ht_next_entry(&it)) {
        STEntry* data = it.data;
        fprintf(fp, "%s\t\t%s\t\t[%d-%d]\t\t", data->variableName, root->moduleName, data->declarationLineNumber, root->lineNumber.end);
        char data_type[20];
        if(data->type == TYPE_INTEGER) strcpy(data_type, "integer");
        else if(data->type == TYPE_REAL) strcpy(data_type, "real");
        else if(data->type == TYPE_BOOLEAN) strcpy(data_type, "boolean");
        else strcpy(data_type, "error_type");
        char is_array[10]; 
        if(data->isArray) strcpy(is_array, "yes");
        else strcpy(is_array, "no");
        char array_type[10];
        if(data->isArray) {
            if(data->isDynamic.lower || data->isDynamic.upper) strcpy(array_type, "dynamic");
            else strcpy(array_type, "static");
        } else {
            strcpy(array_type, "**");
        }
        fprintf(fp, "%s\t\t%s\t\t%s\t\t", data_type, is_array, array_type);
        // Array Range
        if(data->isArray) {
            if(data->isDynamic.lower) {
                fprintf(fp, "[%s-", data->range.lower.lexeme);
            } else {
                fprintf(fp, "[%d-", data->range.lower.value);
            }
            if(data->isDynamic.upper) {
                fprintf(fp, "%s]\t\t", data->range.upper.lexeme);
            } else {
                fprintf(fp, "%d]\t\t", data->range.upper.value);
            }
        } else {
            fprintf(fp, "**\t\t");
        }
        fprintf(fp, "%d\t\t%d\t\t", data->width, data->offset);
        int nest = root->nestingLevel;
        if(data->isOutputParameter) {
            nest--;
        }
        fprintf(fp, "%d\n", nest);
    }
    STTreeNode* temp = root->leftMostChild;
    while(temp != NULL) {
        recursive_print_symbol_table(temp, fp);
        temp = temp->sibling;
    }
}

void print_symbol_table(STTreeNode* root, FILE* fp) {
    fprintf(fp, "variable name\tscope(module name)\tscope(line numbers)\ttype of element\t");
    fprintf(fp, "is_array\tstatic/dynamic array\trange\twidth\toffset\tnesting level\n");
    recursive_print_symbol_table(root, fp);
}

/*
------------------------------------------------------------------------------------
Symbol Table is over here. The code below is for semantic analyser. But I am adding
it in the same file right now to reduce complications in multiple file sync between
me and Shivam.
------------------------------------------------------------------------------------
*/

typedef struct FUNCTION_TRACE Function_Trace;
struct FUNCTION_TRACE {
    char module_name[21];
    Function_Trace* next;
    Function_Trace* prev;
};
Function_Trace* function_trace_stack; // NEVER EMPTY. Contains atleast the driver module.
/*
Note : CLARIFY WITH MA'AM IF SOME MODULE CAN HAVE THE NAME "driver" OR NOT. 
CURRENTLY I AM CONSIDERING THAT IT IS NOT ALLOWED.
IF IT IS ALLOWED, SIMPLY CHANGE THE DRIVER MODULE NAME TO "driverX" where X is any symbol that 
can never be used in another module's name!!!
*/

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

void throw_function_is_recursive_error(char module_name[], int line_no) {
    printf("Error at line %d: Cannot call module %s, recursive calls are not allowed!\n", line_no, module_name);
}

void throw_error() {
    // Not yet implemented. 
    printf("There is an error but the print statement has not yet been implemented\n");
}

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
        FunctionSTEntry* func = ht_fetch(functionST, module_name);
        if(func == NULL || func->declaration_line_no > line_no) {
            // Function has not been declared previously
            // Note : Can add seperate errors for not declared at all, and for not declared YET.
            throw_error();
        } else {
            // Function has been declared previously
            if(function_is_recursive(module_name)) {
                // Recursion.
                throw_error();
            } else {
                // No recursion.

                // 1. Check output parameter list.
                if(func->outputParamList->size == 0) {
                    if(root->child_1 != NULL) {
                        // Function is not returning any parameters but is being assigned to something.
                        throw_error();
                    }
                } else {
                    if(root->child_1 == NULL) {
                        // Function is returning parameters but they are not being assigned to anything.
                        throw_error();
                    } else {
                        ParamListNode* temp1 = func->outputParamList->first;
                        Ast_Node* temp2 = root->child_1->child_1;
                        while(temp1 != NULL && temp2 != NULL) {
                            assert(temp2->type == 33);
                            // Check that the variable temp2 has been declared.
                            STEntry* actual = recursiveCheckID(temp2->symbol_table, temp2->child_1->token_data);
                            if(actual == NULL) {
                                // Variable has not been declared, but is being used.
                                throw_error();
                            } else if(actual->type != temp1->entry->type || actual->isArray) {
                                // Variable types do not match!
                                throw_error();
                            } else if(actual->is_for_loop_variable) {
                                // Variable is a for loop variable - assignment not allowed.
                                throw_error();
                            }
                            temp1 = temp1->next;
                            temp2 = temp2->syn_next;
                        }
                        if(temp1 == NULL && temp2 != NULL) {
                            // Function returns LESS parameters than are being assigned.
                            throw_error();
                        }
                        if(temp1 != NULL && temp2 == NULL) {
                            // Function returns MORE parameters than are being assigned.
                            throw_error();
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
                        throw_error();
                    } else {
                        if(actual->type != temp1->entry->type || actual->isArray != temp1->entry->isArray) {
                            // Type mismatch.
                            throw_error();
                        } else {
                            // NOTE : UNIMPLEMENTED CODE FOR LOWER/UPPER BOUND CHECKING.
                        }
                    }
                    temp1 = temp1->next;
                    temp2 = temp2->syn_next;
                }
                if(temp1 == NULL && temp2 != NULL) {
                    // Function asks for LESS parameters than are being given.
                    throw_error();
                }
                if(temp1 != NULL && temp2 == NULL) {
                    // Function asks for MORE parameters than are being given.
                    throw_error();
                }
            }
        }
    }
    functionChecker(root->child_1);
    functionChecker(root->child_2);
    functionChecker(root->child_3);
    functionChecker(root->child_4);
    functionChecker(root->child_5);
    functionChecker(root->syn_next);
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
}