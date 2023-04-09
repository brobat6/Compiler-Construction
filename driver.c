/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "typeCheck.h"

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

int main(int argc, char *argv[]) {
    printf("FIRST and FOLLOW set automated.\n");
    printf("Both lexical and syntax analysis modules implemented.\n");
    printf("Modules work with all testcases.\n");
    printf("Parse tree constructed.\n");
    printf("As far as our team is aware, everything is running properly.\n");

    ht* lookup_table = initialize();
    FILE* fp=fopen(argv[1],"r+");
    if (fp == NULL) {
        printf("Test file doesn't exist!\n");
        exit(0);
    }
    while(1)
    {
        printf("0. For exit\n");
        printf("1. For removal of comments\n");
        printf("2. For printing the token list (on the console) generated by the lexer\n");
        printf("3. For parsing to verify the syntactic correctness of the input source code and printing the parse tree appropriately\n");
        printf("4. For printing (on the console) the total time taken by your stage 1 code of lexer and parser to verify the syntactic correctness\n");
        int choice=0;

        scanf("%d",&choice);
        if(choice==0) break;
        else if(choice==1){
            //Remove Comments
            removeComments(fp);
        }
        else if(choice==2){
            //Print Token List (Only Lexer operates in this stage.)
            printLexemes(fp, atoi(argv[3]), lookup_table);
        }
        else if(choice==3){
            //Error Printing and Parse Tree Generation
            start_lexer(fp, atoi(argv[3]),true);
            treeNode* parseTreeRoot = parseInputSourceCode(lookup_table, fp, true);
            printParseTree(parseTreeRoot,argv[2]);
            Ast_Node* ast_root = generateAST(parseTreeRoot->firstchild, NULL);
            traverseAST(ast_root, NULL);
            typecheckdfs(ast_root);
        }
        else if(choice==4)
        {
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();

            // Invoke your lexer and parser here
            start_lexer(fp, atoi(argv[3]),false); //Lexer Invoked
            parseInputSourceCode(lookup_table, fp, false); //Parser Invoked   

            end_time = clock();
            total_CPU_time = (double) (end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time /CLOCKS_PER_SEC;
            printf("Total CPU Time : %2.6f\n",total_CPU_time);
            printf("Total CPU Time in seconds: %2.6f\n",total_CPU_time_in_seconds);
        }
        else {
            printf("Invalid Input!\n");
        }
    }
    deallocateStructures(lookup_table);
    fclose(fp);
}