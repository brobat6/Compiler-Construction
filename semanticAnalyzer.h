#include "typeCheck.h"

#ifndef _SEMANTIC_ANALYZER_H
#define _SEMANTIC_ANALYZER_H

typedef struct FUNCTION_TRACE Function_Trace;
struct FUNCTION_TRACE {
    char module_name[21];
    Function_Trace* next;
    Function_Trace* prev;
};
/*
Note : CLARIFY WITH MA'AM IF SOME MODULE CAN HAVE THE NAME "driver" OR NOT. 
CURRENTLY I AM CONSIDERING THAT IT IS NOT ALLOWED.
IF IT IS ALLOWED, SIMPLY CHANGE THE DRIVER MODULE NAME TO "driverX" where X is any symbol that 
can never be used in another module's name!!!
UPD : Clarified with ma'am. It is NOT allowed to name any module as driver.
*/

void push_function_record(char module_name[]);
void pop_function_record();
bool function_is_recursive(char module_name[]);
void throw_function_is_recursive_error(char module_name[], int line_no);
void functionChecker(Ast_Node* root);
void semanticAnalyzer(Ast_Node* root);
void redundant_call_checker();

#endif