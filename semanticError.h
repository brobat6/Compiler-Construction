/*
HOW TO ADD ERRORS

Error e;
e.type = ERROR_VARIABLE_NOT_INIT;
strcpy(e.id_name, "x");
add_error(e);
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#ifndef _SEMANTIC_ERROR_H
#define _SEMANTIC_ERROR_H

typedef enum Error_Type{
    // Shivam errors
    ERROR_INCOMPATIBLE_ARITHMETIC_OPERATION,
    ERROR_INCOMPATIBLE_RELATIONAL_OPERATION,
    ERROR_INCOMPATIBLE_LOGICAL_OPERATION,
    ERROR_INCOMPATIBLE_ASSIGNMENT_OPERATION,
    ERROR_UNKNOWN_OPERATOR,
    ERROR_ARRAY_OUT_OF_BOUNDS,
    VAR_NOT_INITIALIZED,
    VAR_ASSIGNMENT_TO_FOR_LOOP_VARIABLE,
    UNARY_ASSIGNMENT_TO_NON_NUM_RNUM,
    UNKNOWN_TERMINAL_TYPE,
    INVALID_SWITCH_TYPE_FOR_CASES,
    DEFAULT_CASE_IN_BOOLEAN_SWITCH,
    DEFAULT_CASE_IN_INTEGER_SWITCH,
    CASE_NOT_INTEGER_TYPE_FOR_INTEGER_SWITCH,
    CASE_NOT_BOOLEAN_TYPE_FOR_BOOLEAN_SWITCH,
    MISSING_TRUE_FALSE_CASES,
    MISSING_TRUE_CASES,
    MISSING_FALSE_CASES,
    UNEXPECTED_FLAG_VALUE,
    VAR_NOT_OF_TYPE_ARRAY,
    NON_INTEGER_ARRAY_INDEX,
    ERROR_INCOMPATIBLE_ARRAY_ASSIGNMENT_OPERATION,
    ERROR_INCOMPATIBLE_ARRAY_OPERATION,
    ERROR_UNEQUAL_ARRAY_SIZES,
    // Tanveer errors - Symbol Table
    ERROR_MODULE_REDECLARATION,
    ERROR_MODULE_REDEFINITION,
    ERROR_IDENTIFIER_ALREADY_EXISTS,
    // Tanveer errors - Semantic Analyzer
    ERROR_FUNCTION_RECURSIVE,
    ERROR_FUNCTION_OUTPUT_NOT_BEING_ASSIGNED, // don't confuse with function output not being assigned anything! this is for function CALL
    ERROR_FUNCTION_OUTPUT_BEING_ASSIGNED_EXTRA,
    ERROR_FUNCTION_OUTPUT_TYPES_DONT_MATCH,
    ERROR_FUNCTION_OUTPUT_ARRAY,
    ERROR_FUNCTION_INPUT_TYPES_DONT_MATCH,
    ERROR_FUNCTION_GETS_LESS_INPUT,
    ERROR_FUNCTION_GETS_MORE_INPUT,
    ERROR_FUNCTION_NOT_DECLARED,
    ERROR_FUNCTION_RETURN_PARAMETER_NOT_BEING_ASSIGNED, // if a return parameter is not assigned any value in the duration of the function
    ERROR_REDUNDANT_FUNCTION_DECLARATION

}Error_Type;

typedef struct ERROR Error;
struct ERROR {
    Error_Type type;
    int line;
    char module_name[21];
    char id_name[21];
    // Make nodes for additional data here.
    int line_2;
    char module_name_2[21];
    char id_name_2[21];
};

typedef struct ERROR_LIST Error_List;
struct ERROR_LIST {
    Error error;
    Error_List* next;
    Error_List* prev;
};

void add_error(Error error);

void print_semantic_errors(FILE *fp);

void reset_error_list();

int number_of_semantic_errors();

#endif