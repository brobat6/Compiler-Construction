#include "semanticAnalyzer.h"

ht* initialize () {
    readGrammarFile();
    computeFirstAndFollowSets();
    createParseTable();
    ht* lookup_table = init_ht();
    if(lookup_table == NULL) exit_nomem();
    fill_lookup_table(lookup_table);
    // print_lookup_table(lookup_table);
    return lookup_table;
}

void deallocateStructures (ht* lookup_table) {
    delete_ht(lookup_table);
    deallocateBuff();
    deallocateFromParser();
}

int main(int argc, char* argv[]) {



    while(true) {
        printf("0. To exit from the loop.\n");
        printf("1. Lexer : For printing the token list generated by the lexer.\n");
        printf("2. Parser : For parsing to verify the syntactic correctness of the input source code and to produce parse tree.\n");
        printf("3. AST : For printing the Abstract Syntax Tree in appropriate format.\n");
        printf("4. Memory: For displaying the amount of allocated memory and number of nodes to each of parse tree and abstract syntax tree for the test case used.\n");
        printf("5. Symbol Table : For printing the symbol table.\n");
        printf("6. Activation Record Size : For printing the total memory requirement for each function.\n");
        printf("7. Static and dynamic arrays : For printing the type expressions and width of array variables in a line.\n");
        printf("8. Errors reporting and total compiling time.\n");
        printf("9. Code generation : For producing assembly code.\n");

        int choice;
        scanf("%d", &choice);

        switch(choice)
        {
        case 0:
            
            break;
        case 1:

            break;
        case 2:

            break;
        case 3:

            break;
        case 4:

            break;
        case 5:

            break;
        case 6:

            break;
        case 7:

            break;
        case 8:

            break;
        case 9:

            break;
        default:
            printf("Invalid input. Please specify a choice between 0 and 9 only.\n");
        }
    }
}