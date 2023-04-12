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
    ERROR_UNEQUAL_ARRAY_SIZES
}Error_Type;

typedef struct ERROR Error;
struct ERROR {
    Error_Type type;
    int line;
    char module_name[21];
    char id_name[21];
    // Make nodes for additional data here.
};

typedef struct ERROR_LIST Error_List;
struct ERROR_LIST {
    Error error;
    Error_List* next;
    Error_List* prev;
};

void add_error(Error error);

void print_semantic_errors(FILE *fp);

#endif