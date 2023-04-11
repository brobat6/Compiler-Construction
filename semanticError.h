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

typedef enum Error_Type{
    ERROR_VARIABLE_NOT_INIT,
    ERORR_BLAH_BLAH
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
