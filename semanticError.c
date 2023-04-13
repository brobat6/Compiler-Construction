#include "semanticError.h"

Error_List* error_list;

void add_error(Error error) {
    Error_List* new_error = (Error_List*)malloc(sizeof(Error_List));
    new_error->error = error;
    if(error_list == NULL) {
        error_list = new_error;
        error_list->prev = NULL;
        error_list->next = NULL;
        return;
    }
    if(error_list->error.line > new_error->error.line) {
        new_error->next = error_list;
        new_error->prev = NULL;
        error_list->prev = new_error;
        error_list = new_error;
        return;
    }
    Error_List* prev = error_list;
    Error_List* next = error_list->next;
    while(next != NULL && next->error.line <= new_error->error.line) {
        prev = prev->next;
        next = next->next;
    }
    if(next == NULL) {
        new_error->next = NULL;
        new_error->prev = prev;
        prev->next = new_error;
    } else {
        prev->next = new_error;
        new_error->next = next;
        next->prev = new_error;
        new_error->prev = prev;
    }
}

void print_semantic_errors(FILE *fp) {
    Error_List* e = error_list;
    if(e == NULL) {
        fprintf(fp, "There are no semantic errors in the code!\n");
        return;
    }
    fprintf(fp, "List of semantic errors:\n");
    while(e != NULL) {
        fprintf(fp, "Line : %d, Error : ", e->error.line);
        
        // Shivam Errors
        if(e->error.type == ERROR_INCOMPATIBLE_ARITHMETIC_OPERATION) {
            fprintf(fp, "Type mismatch about arithmetic operator '%s'.", e->error.id_name);
        }
        else if(e->error.type == ERROR_INCOMPATIBLE_RELATIONAL_OPERATION) {
            fprintf(fp, "Type mismatch about relational operator '%s'.", e->error.id_name);
        }
        else if(e->error.type == ERROR_INCOMPATIBLE_LOGICAL_OPERATION) {
            fprintf(fp, "Type mismatch about logical operator '%s'.", e->error.id_name);
        }
        else if(e->error.type == ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION) {
            fprintf(fp, "Type mismatch error about assignment operator ':='.");
        }
        else if(e->error.type == ERROR_UNKNOWN_OPERATOR) {
            fprintf(fp, "Unknown operator type.");
        }
        else if(e->error.type == ERROR_ARRAY_OUT_OF_BOUNDS) {
            fprintf(fp, "Element of array %s is out of bounds.", e->error.id_name);
        }
        else if(e->error.type == VAR_NOT_INITIALIZED) {
            fprintf(fp, "Variable %s is not declared.", e->error.id_name);
        }
        else if(e->error.type == VAR_ASSIGNMENT_TO_FOR_LOOP_VARIABLE) {
            fprintf(fp, "For loop variable %s cannot be assigned any value.", e->error.id_name);
        }
        else if(e->error.type == UNARY_ASSIGNMENT_TO_NON_NUM_RNUM) {
            fprintf(fp, "Type mismatch error while applying unary operator.");
        }
        else if(e->error.type == UNKNOWN_TERMINAL_TYPE) {
            fprintf(fp, "Terminal type unknown.");
        }
        else if(e->error.type == INVALID_SWITCH_TYPE_FOR_CASES) {
            fprintf(fp, "Switch variable %s cannot be of type real.", e->error.id_name);
        }
        else if(e->error.type == DEFAULT_CASE_IN_BOOLEAN_SWITCH) {
            fprintf(fp, "Presence of default statement is incorrect as condition variable type is boolean.");
        }
        else if(e->error.type == DEFAULT_CASE_IN_INTEGER_SWITCH) {
            fprintf(fp, "Default statement is missing - the type of switch variable is integer.");
        }
        else if(e->error.type == CASE_NOT_INTEGER_TYPE_FOR_INTEGER_SWITCH) {
            fprintf(fp, "Case value is incorrect as condition variable type is integer.");
        }
        else if(e->error.type == CASE_NOT_BOOLEAN_TYPE_FOR_BOOLEAN_SWITCH) {
            fprintf(fp, "Case value is incorrect as condition variable type is boolean.");
        }
        else if(e->error.type == MISSING_TRUE_FALSE_CASES) {
            fprintf(fp, "Case value true and false are missing as condition variable type is boolean.");
        }
        else if(e->error.type == MISSING_TRUE_CASES) {
            fprintf(fp, "Case value true is missing as condition variable type is boolean.");
        }
        else if(e->error.type == MISSING_FALSE_CASES) {
            fprintf(fp, "Case value false is missing as condition variable type is boolean.");
        }
        else if(e->error.type == UNEXPECTED_FLAG_VALUE) {
            fprintf(fp, "Unexpected error has occurred.");
        }
        else if(e->error.type == VAR_NOT_OF_TYPE_ARRAY) {
            fprintf(fp, "Variable %s is not of type array.", e->error.id_name);
        }
        else if(e->error.type == NON_INTEGER_ARRAY_INDEX) {
            fprintf(fp, "Array index does not evaluate to type integer");
        }
        else if(e->error.type == ERROR_INCOMPATIBLE_ARRAY_ASSIGNMENT_OPERATION) {
            fprintf(fp, "Type mismatch - Expression in RHS cannot be assigned to array %s.", e->error.id_name);
        }
        else if(e->error.type == ERROR_INCOMPATIBLE_ARRAY_OPERATION) {
            fprintf(fp, "Type mismatch - cannot operate on the array %s as a whole.", e->error.id_name);
        }
        else if(e->error.type == ERROR_UNEQUAL_ARRAY_SIZES) {
            fprintf(fp, "Type mismatch - the arrays %s and %s do not have the same size.", e->error.id_name, e->error.id_name_2);
        }
        // Tanveer Errors
        else if(e->error.type == ERROR_MODULE_REDECLARATION) {
            fprintf(fp, "Module %s already exists (previously declared at line %d).", e->error.module_name, e->error.line_2);
        }
        else if(e->error.type == ERROR_MODULE_REDEFINITION) {
            fprintf(fp, "Module %s cannot be overloaded.", e->error.module_name);
        }
        else if(e->error.type == ERROR_IDENTIFIER_ALREADY_EXISTS) {
            fprintf(fp, "Variable %s already exists (previously declared at line %d).", e->error.id_name, e->error.line_2);
        }
        else if(e->error.type == ERROR_FUNCTION_RECURSIVE) {
            fprintf(fp, "Recursive function call.");
        }
        else if(e->error.type == ERROR_FUNCTION_OUTPUT_NOT_BEING_ASSIGNED) {
            fprintf(fp, "Number of parameters returned by module %s is MORE than the number of parameters on LHS of assignment.", e->error.module_name);
        }
        else if(e->error.type == ERROR_FUNCTION_OUTPUT_BEING_ASSIGNED_EXTRA) {
            fprintf(fp, "Number of parameters returned by module %s is LESS than the number of parameters on LHS of assignment.", e->error.module_name);
        }
        else if(e->error.type == ERROR_FUNCTION_OUTPUT_TYPES_DONT_MATCH) {
            fprintf(fp, "Type mismatch between formal output parameter %s and actual output parameter %s.", e->error.id_name_2, e->error.id_name);
        }
        else if(e->error.type == ERROR_FUNCTION_OUTPUT_ARRAY) {
            fprintf(fp, "%s is an array, it can never be an output parameter of a function.", e->error.id_name);
        }
        else if(e->error.type == ERROR_FUNCTION_INPUT_TYPES_DONT_MATCH) {
            fprintf(fp, "Type mismatch between formal input parameter %s and actual input parameter %s.", e->error.id_name_2, e->error.id_name);
        }
        else if(e->error.type == ERROR_FUNCTION_GETS_LESS_INPUT) {
            fprintf(fp, "Module %s takes LESS input parameters than its definition.", e->error.module_name);
        }
        else if(e->error.type == ERROR_FUNCTION_GETS_MORE_INPUT) {
            fprintf(fp, "Module %s takes MORE input parameters than its definition.", e->error.module_name);
        }
        else if(e->error.type == ERROR_FUNCTION_NOT_DECLARED) {
            fprintf(fp, "Module %s has not been declared but is being called.", e->error.module_name);
        }
        else if(e->error.type == ERROR_FUNCTION_RETURN_PARAMETER_NOT_BEING_ASSIGNED) {
            fprintf(fp, "Output parameter %s is not assigned any value in the entire module %s.", e->error.id_name, e->error.module_name);
        }
        else if(e->error.type == ERROR_REDUNDANT_FUNCTION_DECLARATION) {
            fprintf(fp, "Module %s definition and its declaration both appear before its call.", e->error.module_name);
        }
        else {
            fprintf(fp, "There is an error but the print statement has not been implemented yet.");
        }
        e = e->next;
        fprintf(fp, "\n");
    }
    fprintf(fp, "\n");
}
