#include "symbolTable.h"

#define DATASEGMENTOFFSET 1000
int curOffset=DATASEGMENTOFFSET;
STTreeNode* curSTTreeNode=NULL;
STTreeNode* globalSTRoot=NULL;
STEntry* tempEntry;
ht* functionST;

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
    }
    return newNode;
}

STEntry* checkID(STTreeNode* node,char lexeme[]){      //confirm input parameter
    return ht_fetch(node->hashTable,lexeme);
}

FunctionSTEntry* checkFunctionID(char lexeme[]) {
    return ht_fetch(functionST, lexeme);
}

STEntry* recursiveCheckID(STTreeNode* node,Token_Info* t){         //confirm input parameter
    char lexeme[21];
    strcpy(lexeme,t->lexeme);
    if(checkID(node,lexeme)==NULL&&node->parent!=NULL){
        return recursiveCheckID(node->parent,t);
    }
    else if(checkID(node,lexeme)!=NULL&&checkID(node,lexeme)->declarationLineNumber>t->lineNumber){
        return recursiveCheckID(node->parent,t);
    }
    return checkID(node,lexeme);
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
    tempEntry->isParameter = false;
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
    printf("Function %s is being defined at line %d, but it has not been declared!\n", module_name, line_no);
}

void throw_function_redefinition_error(char module_name[], int line_no) {
    printf("Function %s being redefined at %d, has been defined previously.\n", module_name, line_no);
}

void generateST(STTreeNode* currSTNode, Ast_Node* root) {

    assert(root != NULL);
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
        int start_line_no = root->token_data->lineNumber; // DEF
        int end_line_no = root->child_4->child_3->token_data->lineNumber; // <moduleDef> ---> END
        STTreeNode* childSTNode = createSTTreeNode();
        addSTChild(currSTNode, childSTNode);
        (childSTNode->lineNumber).begin = start_line_no;
        (childSTNode->lineNumber).end = end_line_no;
        strcpy(childSTNode->moduleName, root->child_1->token_data->lexeme);
        childSTNode->offset = curOffset;

        // 3. Populate input_plist and output_plist
        func->inputParamList = initialize_parameter_list();
        func->outputParamList = initialize_parameter_list();

        Ast_Node* temp = root->child_2;
        while(temp != NULL) {
            populate_temporary_function_entry(temp->child_2);
            STEntry* entry = createSTEntry(tempEntry);
            entry->isParameter = true;
            strcpy(entry->variableName, temp->child_1->token_data->lexeme);
            entry->declarationLineNumber = temp->child_1->token_data->lineNumber;
            entry->offset = curOffset;
            curOffset += entry->width;
            // Currently adding input/output lists in CHILD symbol table.
            if(checkID(childSTNode, entry->variableName) != NULL) {
                throw_already_exists_error(entry->variableName, entry->declarationLineNumber);
            } else {
                ht_store(childSTNode->hashTable, entry->variableName, entry);
                add_to_parameter_list(func->inputParamList, entry);
            }
            temp = temp->syn_next;
        }
        temp = root->child_3;
        if(temp != NULL) temp = temp->child_1;
        while(temp != NULL) {
            populate_temporary_function_entry(temp->child_2);
            STEntry* entry = createSTEntry(tempEntry);
            entry->isParameter = true;
            strcpy(entry->variableName, temp->child_1->token_data->lexeme);
            entry->declarationLineNumber = temp->child_1->token_data->lineNumber;
            entry->offset = curOffset;
            curOffset += entry->width;
            // Currently adding input/output lists in CHILD symbol table.
            // Change if necessary !!!!!!
            if(checkID(childSTNode, entry->variableName) != NULL) {
                throw_already_exists_error(entry->variableName, entry->declarationLineNumber);
            } else {
                ht_store(childSTNode->hashTable, entry->variableName, entry);
                add_to_parameter_list(func->outputParamList, entry);
            }
            temp = temp->syn_next;
        }
        generateST(childSTNode, root->child_4);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        func->function_width = childSTNode->nodeWidth;
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
        generateST(childSTNode, root->child_3);
        generateST(childSTNode, root->child_4);
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

        // 3. Call with child symbol table inside for loop
        generateST(currSTNode, root->child_2); /*
        1. Doing this for type checking etc later, as symbol table needs to be populated
        2. What happens in case we have FOR X IN RANGE [X...3], i.e. range has same identifier declared previously?
        Current ans. It would not give error, the X in range would be taken from outer scope. 
        */
        generateST(childSTNode, root->child_4);
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
        generateST(childSTNode, root->child_3);
        childSTNode->nodeWidth = (curOffset - childSTNode->offset);
        return;
    }
    
    if(root->child_1 != NULL) generateST(currSTNode, root->child_1);
    if(root->child_2 != NULL) generateST(currSTNode, root->child_2);
    if(root->child_3 != NULL) generateST(currSTNode, root->child_3);
    if(root->child_4 != NULL) generateST(currSTNode, root->child_4);
    if(root->child_5 != NULL) generateST(currSTNode, root->child_5);
    if(root->syn_next != NULL) generateST(currSTNode, root->syn_next);
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
                fprintf(fp, "%s]\t\t", data->range.lower.lexeme);
            } else {
                fprintf(fp, "%d]\t\t", data->range.lower.value);
            }
        } else {
            fprintf(fp, "**\t\t");
        }
        fprintf(fp, "%d\t\t%d\t\t", data->width, data->offset);
        int nest = root->nestingLevel;
        if(data->isParameter) {
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

