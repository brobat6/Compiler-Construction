/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "parser.h"

int BUF_LIMIT = 101;
int MAX_RULE_COUNT = 150;
int MAXIMUM_OCCURENCES_ON_RHS = 20;
bool errorPrint = true;
int isError = 0;
ht* terminal_ht = NULL;
ht* nonterminal_ht = NULL;
llHead** grammar = NULL;
int rule_count = 0;
llHead*** parse_table = NULL;

void readGrammarFile () {
    FILE* grammar_fp = fopen("grammar.txt", "r");
    grammar = (llHead**) calloc(MAX_RULE_COUNT, sizeof(llHead*));
    char buf[BUF_LIMIT];
    terminal_ht = init_ht();
    nonterminal_ht = init_ht();
    grammar_symbol* dollar_terminal = initialize_grammar_symbol(dollar_terminal, 2);
    dollar_terminal->terminal = 1;
    strcpy (dollar_terminal->name, "$");
    ht_store(terminal_ht, "$", dollar_terminal);

    grammar_symbol* syn_terminal = initialize_grammar_symbol(syn_terminal, 6);
    syn_terminal->terminal = 1;
    strcpy (syn_terminal->name, "#syn#");
    ht_store(terminal_ht, "#syn#", syn_terminal);
    
    while(fgets(buf, BUF_LIMIT - 1, grammar_fp) != NULL) {
        char cur_id[BUF_LIMIT];
        int cur_buf_ptr = 0, cur_id_ptr = 0, cur_len = strlen(buf);
        grammar_symbol *cur_head = NULL, *cur_node = NULL;
        llHead *cur_ll_head = NULL;
        llNode *cur_ll_node = NULL, *prev_ll_node = NULL;
        int arrow = 0;
        while (cur_buf_ptr < cur_len) {
            if (buf[cur_buf_ptr] == '@') {
                // Must be epsilon
                cur_node = ht_fetch(terminal_ht, "@");
                if (cur_node == NULL) {
                    cur_node = initialize_grammar_symbol(cur_node, 2);
                    cur_node->epsilon = 1;
                    strcpy (cur_node->name, "@");
                    ht_store(terminal_ht, "@", cur_node);
                }
                cur_ll_node = initializeNode();
                cur_ll_node->data = cur_node;
                cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                cur_ll_node = NULL;
            }
            else if (buf[cur_buf_ptr] == ' ' || buf[cur_buf_ptr] == '\n') {
                if (cur_id_ptr != 0) {
                    // Must be a terminal
                    cur_id[cur_id_ptr] = '\0';
                    int cur_len = strlen(cur_id);
                    cur_node = ht_fetch(terminal_ht, cur_id);
                    if (cur_node == NULL) {
                        cur_node = initialize_grammar_symbol(cur_node, cur_len + 1);
                        cur_node->terminal = 1;
                        strcpy (cur_node->name, cur_id);
                        ht_store(terminal_ht, cur_id, cur_node);
                    }
                    cur_ll_node = initializeNode();
                    cur_ll_node->data = cur_node;
                    cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                    cur_ll_node = NULL;
                    cur_id_ptr = 0;
                }
            }
            else if (buf[cur_buf_ptr] == '-') {
                arrow = 1;
            }
            else if (buf[cur_buf_ptr] == '>') {
                if (arrow == 1) {
                    arrow = 0;
                }
                else {
                    // Must be a non terminal
                    cur_id[cur_id_ptr] = buf[cur_buf_ptr];
                    cur_id_ptr++;
                    cur_id[cur_id_ptr] = '\0';
                    int cur_len = strlen(cur_id);
                    cur_node = ht_fetch(nonterminal_ht, cur_id);
                    if (cur_node == NULL) {
                        cur_node = initialize_grammar_symbol(cur_node, cur_len + 1);
                        cur_node->non_terminal = 1;
                        strcpy (cur_node->name, cur_id);
                        ht_store(nonterminal_ht, cur_id, cur_node);
                    }
                    if (cur_ll_head == NULL) {
                        if (cur_node->start == -1) {
                            cur_node->start = rule_count;
                        }
                        cur_node->end = rule_count;
                        cur_ll_head = initializeHead();
                        cur_ll_head->data = cur_node;
                        grammar[rule_count] = cur_ll_head;
                        cur_ll_head->grammar_rule = rule_count;
                    }
                    else {
                        cur_ll_node = initializeNode();
                        cur_ll_node->data = cur_node;
                        cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                        if (cur_node->occurences == NULL) {
                            cur_node->occurences = calloc(MAXIMUM_OCCURENCES_ON_RHS, sizeof(llNode*));
                            cur_node->occurence_rule = calloc(MAXIMUM_OCCURENCES_ON_RHS, sizeof(int));
                        }
                        int cur_occurence_count = cur_node->occurence_count;
                        cur_node->occurences[cur_occurence_count] = cur_ll_node;
                        cur_node->occurence_rule[cur_occurence_count] = rule_count;
                        cur_node->occurence_count++;
                        cur_ll_node = NULL;
                    }
                    cur_id_ptr = 0;
                }
            }
            else {
                cur_id[cur_id_ptr] = buf[cur_buf_ptr];
                cur_id_ptr++;
            }
            cur_buf_ptr++;
        }
        rule_count++;
    }
    fclose(grammar_fp);

    // For printing the stored grammar
    // for (int i=0;i<rule_count;i++) {
    //     grammar_symbol* cur_data = grammar[i]->data;
    //     printf("%d. %s ->", i, cur_data->name);
    //     llNode* cur_node = grammar[i]->first_node;
    //     while (cur_node != NULL) {
    //         cur_data = cur_node->data;
    //         cur_node = cur_node->next;
    //         printf(" %s", cur_data->name);
    //     }
    //     printf("\n");
    // }
}

void deallocateNode (llNode* node) {
    if (node == NULL) {
        return;
    }
    grammar_symbol* symbol = node->data;
    free(node);
}

void deallocateHead (llHead* head) {
    if (head == NULL) {
        return;
    }
    grammar_symbol* symbol = head->data;
    free(head);
}

void deallocateGrammar () {
    for (int i= 0; i <= rule_count; i++) {
        llHead* cur_head = grammar[i];
        llNode* cur_node = cur_head->first_node;
        while (cur_node != NULL) {
            llNode* temp = cur_node;
            cur_node = cur_node->next;
            deallocateNode(temp);
        }
        deallocateHead(cur_head);
    }
    free(grammar);
}

void findFirstSet (char* cur_id) {
    grammar_symbol* cur_grammar_symbol = ht_fetch(terminal_ht, cur_id);
    if (cur_grammar_symbol != NULL) {
        if (cur_grammar_symbol->first_set != NULL) return;
        int cur_terminal_pos = ht_fetch_index(terminal_ht, cur_id);
        cur_grammar_symbol->first_set = calloc(2, sizeof(unsigned long long));
        cur_grammar_symbol->first_set[cur_terminal_pos/64] = (1ull<<(cur_terminal_pos%64));
        return;
    }
    cur_grammar_symbol = ht_fetch(nonterminal_ht, cur_id);
    if (cur_grammar_symbol->first_set != NULL) return;
    int start_rule = cur_grammar_symbol->start;
    int end_rule = cur_grammar_symbol->end;
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    unsigned long long* cur_first_set = calloc(2, sizeof(unsigned long long));
    cur_first_set[0] = 0ull;
    cur_first_set[1] = 0ull;
    for (int i = start_rule; i <= end_rule; i++) {
        unsigned long long cur_rule_first_set[2] = {0ull, 0ull};
        llNode* cur_llNode = grammar[i]->first_node;
        while (cur_llNode != NULL) {
            grammar_symbol* cur_node = cur_llNode->data;
            if (cur_node->first_set == NULL) {
                findFirstSet (cur_node->name);
            }
            int isEpsilon = cur_node->first_set[epsilon_pos/64] & epsilon;
            cur_rule_first_set[0] = cur_rule_first_set[0] | (cur_node->first_set[0]);
            cur_rule_first_set[1] = cur_rule_first_set[1] | (cur_node->first_set[1]);
            if (isEpsilon) {
                cur_rule_first_set[epsilon_pos/64] ^= epsilon;
            }
            if (cur_node->terminal) {
                break;
            }
            if ((cur_node->first_set[epsilon_pos/64] & epsilon) == 0ull) {
                break;
            }
            cur_llNode = cur_llNode->next;
        }
        if (cur_llNode == NULL) {
            cur_rule_first_set[epsilon_pos/64] |= epsilon;
        }
        grammar[i]->first_set[0] = cur_rule_first_set[0];
        grammar[i]->first_set[1] = cur_rule_first_set[1];
        cur_first_set[0] = cur_first_set[0] | cur_rule_first_set[0];
        cur_first_set[1] = cur_first_set[1] | cur_rule_first_set[1];
    }
    cur_grammar_symbol->first_set = cur_first_set;
    return;
}

void findFollowSet (char* cur_id) {
    grammar_symbol* cur_grammar_symbol = ht_fetch(nonterminal_ht, cur_id);
    if (cur_grammar_symbol->follow_set != NULL) return;
    int cur_occurence_count = cur_grammar_symbol->occurence_count;
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    unsigned long long* cur_follow_set = calloc(2, sizeof(unsigned long long));
    cur_follow_set[0] = 0ull;
    cur_follow_set[1] = 0ull;
    for (int i = 0; i < cur_occurence_count; i++) {
        llNode* cur_llNode = cur_grammar_symbol->occurences[i];
        int cur_occurence_rule = cur_grammar_symbol->occurence_rule[i];
        if (cur_llNode->next == NULL) {
            grammar_symbol* cur_rule_head = grammar[cur_occurence_rule]->data;
            if (strcmp(cur_rule_head->name, cur_grammar_symbol->name) == 0) {
                continue;
            }
            if (cur_rule_head->follow_set == NULL) {
                findFollowSet (cur_rule_head->name);
            }
            cur_follow_set[0] = cur_follow_set[0] | (cur_rule_head->follow_set[0]);
            cur_follow_set[1] = cur_follow_set[1] | (cur_rule_head->follow_set[1]);
        }
        else {
            cur_llNode = cur_llNode->next;
            grammar_symbol* cur_node = cur_llNode->data;
            if (cur_node->epsilon == 1) {
                continue;
            }
            if (cur_node->first_set == NULL) {
                findFirstSet(cur_node->name);
            }
            cur_follow_set[0] = cur_follow_set[0] | (cur_node->first_set[0]);
            cur_follow_set[1] = cur_follow_set[1] | (cur_node->first_set[1]);
            if ((cur_node->first_set[epsilon_pos/64] & epsilon) > 0) {
                cur_follow_set[epsilon_pos/64] ^= epsilon;
                if (cur_node->follow_set == NULL) {
                    findFollowSet (cur_node->name);
                }
                cur_follow_set[0] = cur_follow_set[0] | (cur_node->follow_set[0]);
                cur_follow_set[1] = cur_follow_set[1] | (cur_node->follow_set[1]);
            }
        }
    }
    cur_grammar_symbol->follow_set = cur_follow_set;
    return;
}

void computeFirstAndFollowSets () {
    // Populate first sets
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        findFirstSet (cur_node->name);
        // For printing first sets of every non terminal
        // printf("%s ->", cur_node->name);
        // for (int i=0;i<128;i++) {
        //     if ((cur_node->first_set[i/64] & 1ull<<(i%64)) > 0) {
        //         grammar_symbol* node = terminal_ht->entry[i].data;
        //         printf(" %s", node->name);
        //     }
        // }
        // printf("\n");
        // printf("%llu %llu\n", cur_node->first_set[0], cur_node->first_set[1]);
    }

    // Populate follow sets
    // adding $ to follow set of <program>
    int dollar_index = ht_fetch_index(terminal_ht, "$");
    grammar_symbol* cur_node = grammar[0]->data;
    cur_node->follow_set = calloc(2, sizeof(unsigned long long));
    cur_node->follow_set[dollar_index/64] = 1ull<<(dollar_index%64);
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        findFollowSet (cur_node->name);
        // For printing follow sets of every non terminal
        // printf("%s ->", cur_node->name);
        // for (int i=0;i<128;i++) {
        //     if ((cur_node->follow_set[i/64] & 1ull<<(i%64)) > 0) {
        //         grammar_symbol* node = terminal_ht->entry[i].data;
        //         printf(" %s", node->name);
        //     }
        // }
        // printf("\n");
        // printf("%llu %llu\n", cur_node->follow_set[0], cur_node->follow_set[1]);
    }
}

void createParseTable () {
    int row_count = nonterminal_ht->cap;
    int col_count = terminal_ht->cap;
    parse_table = calloc(row_count, sizeof(llHead**));
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    grammar_symbol* syn_symbol = ht_fetch(terminal_ht, "#syn#");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    grammar[rule_count] = initializeHead();
    llNode* syn_node = initializeNode();
    syn_node->data = syn_symbol;
    insertAtEnd(grammar[rule_count], syn_node);
    for (int i = 0; i < row_count; i++) {
        parse_table[i] = calloc(col_count, sizeof(llHead*));
        for (int j = 0; j < col_count; j++) {
            parse_table[i][j] = NULL;
        }
    }
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        int cur_index = ht_fetch_index(nonterminal_ht, cur_node->name);
        int isEpsilon = 0;
        if ((grammar[i]->first_set[epsilon_pos/64] & epsilon) > 0) {
            isEpsilon = 1;
        }
        for (int j = 0; j < 128; j++) {
            if ((grammar[i]->first_set[j/64] & 1ull<<(j%64)) > 0) {
                if (parse_table[cur_index][j] != NULL) {

                }
                else {
                    parse_table[cur_index][j] = grammar[i];
                }
            }
            if ((cur_node->follow_set[j/64] & 1ull<<(j%64)) > 0) {
                if (isEpsilon == 1) {
                    if (parse_table[cur_index][j] != NULL) {

                    }
                    else {
                        parse_table[cur_index][j] = grammar[i];
                    }
                }
                else if (parse_table[cur_index][j] == NULL && i == cur_node->end) {
                    parse_table[cur_index][j] = grammar[rule_count];
                }
            }
        }
    }
}

void deallocateParseTable () {
    if (parse_table == NULL) return;
    int row_count = nonterminal_ht->cap;
    for (int i = 0; i < row_count; i++) {
        if (parse_table[i])
            free(parse_table[i]);
    }
    free(parse_table);
}

treeNode* parseInputSourceCode (ht* lookup_table, FILE* fp, bool printError) {
    llHead* stack = initializeHead();
    // Inserting $ onto the stack
    grammar_symbol* dollar_terminal = ht_fetch(terminal_ht, "$");
    llNode* dollar_node = initializeNode();
    dollar_node->data = dollar_terminal;
    stack = insertAtEnd(stack, dollar_node);
    // Inserting <program> onto the stack
    grammar_symbol* program_start = grammar[0]->data;
    llNode* program_node = initializeNode();
    struct token_info null_token;
    treeNode* root=createTreeNode(dollar_terminal, null_token);
    treeNode* cur_tree_node=createTreeNode(program_start, null_token);
    addChild(root, cur_tree_node);

    program_node->data = program_start;
    stack = insertAtEnd(stack, program_node);
    isError = 0;
    errorPrint = printError;
    bool wasSemicol = false;
    while (stack->count > 0) {
        struct token_info cur_token = getNextToken(lookup_table, fp);
        // For printing current input token
        // printf("%s\n", tokenToString[cur_token.token]);
        while (stack->count > 0) {
            // For printing current stack state
            // llNode* current = stack->first_node;
            // for (int i = 0; i < stack->count; i++) {
            //     grammar_symbol* gg = current->data;
            //     printf("%s ", gg->name);
            //     current = current->next;
            // }
            // printf("\n");
            grammar_symbol* current_symbol = stack->last_node->data;
            char cur_token_name[20];
            if (cur_token.token == ERROR) {
                // LEXICAL ERROR REPORTED
                break;
            }
            else if (cur_token.token == ENDOFFILE) {
                strcpy(cur_token_name, "$");
            }
            else {
                strcpy(cur_token_name, tokenToString[cur_token.token]);
            }
            if (current_symbol->terminal == 1) {
                if (strcmp(cur_token_name, current_symbol->name) == 0) {
                    if (cur_tree_node) {
                        cur_tree_node->token = cur_token;
                    }
                    wasSemicol = false;
                    removeFromEnd(stack);
                    if (strcmp(cur_token_name, "$") != 0) {
                        cur_tree_node = nextLeaf(cur_tree_node);
                    }
                    break;
                }
                else {
                    if (cur_token.token == ENDOFFILE || wasSemicol == true) {
                        removeFromEnd(stack);
                        cur_tree_node = nextLeaf(cur_tree_node);
                        isError = 1;
                    }
                    else {
                        if (cur_token.token == SEMICOL || cur_token.token == END || cur_token.token == BC) {
                            wasSemicol = true;
                        }
                        if (printError) {
                            printf("\nError -> Line ");
                            printf("%d | Unexpected token %s\n", cur_token.lineNumber, tokenToString[cur_token.token]);
                        }
                        isError = 1;
                        break;
                    }
                }
            }
            else {
                int parse_table_row = ht_fetch_index(nonterminal_ht, current_symbol->name);
                int parse_table_col = ht_fetch_index(terminal_ht, cur_token_name);
                llHead* cur_rule = parse_table[parse_table_row][parse_table_col];
                grammar_symbol* cur_symbol;
                int isEOF = 0;
                if (cur_rule == NULL) {
                    if (cur_token.token != ENDOFFILE && wasSemicol == false) {
                        if (cur_token.token == SEMICOL || cur_token.token == END || cur_token.token == BC) {
                            wasSemicol = true;
                        }
                        if (printError) {
                            printf("\nError -> Line ");
                            printf("%d | Unexpected token %s\n", cur_token.lineNumber, tokenToString[cur_token.token]);
                        }
                        isError = 1;
                        break;
                    }
                    else if (cur_token.token == ENDOFFILE) {
                        isEOF = 1;
                    }
                }
                else {
                    cur_symbol = cur_rule->first_node->data;
                }
                if (cur_rule == NULL || isEOF == 1 || strcmp(cur_symbol->name, "#syn#") == 0) {
                    removeFromEnd(stack);
                    cur_tree_node = nextLeaf(cur_tree_node);
                    isError = 1;
                }
                else {
                    wasSemicol = false;
                    removeFromEnd(stack);
                    cur_tree_node->grammar_rule = cur_rule->grammar_rule;
                    llNode* cur_node = cur_rule->last_node;
                    grammar_symbol* cur_grammar_symbol;

                    treeNode* firsttreenode;
                    int isEpsilon = 0;
                    while (cur_node != NULL) {
                        cur_grammar_symbol = cur_node->data;

                        firsttreenode=createTreeNode(cur_grammar_symbol, null_token);
                        addChild(cur_tree_node,firsttreenode);

                        if (cur_grammar_symbol->epsilon == 1) {
                            isEpsilon = 1;
                            break;
                        }
                        llNode* push_node = initializeNode();
                        push_node->data = cur_grammar_symbol;
                        insertAtEnd(stack, push_node);
                        cur_node = cur_node->prev;
                    }

                    cur_tree_node=firsttreenode;
                    if (isEpsilon) {
                        cur_tree_node = nextLeaf(cur_tree_node);
                    }

                }
            }
        }
    }
    if (printError) {
        if (isError) {
            printf("Error(s) were found during parsing.\n");
        }
        else {
            printf("No error was found during parsing.\n");
        }
    }
    // Stack is empty coming out of the loop
    free(stack);
    return root;
}

void printNodeInfo (treeNode* treenode, FILE* tree_fp) {
    if (treenode->symbol->terminal == 1) {
        struct token_info cur_token = treenode->token;
        fprintf(tree_fp, "%s %d %s ", treenode->symbol->name, cur_token.lineNumber, tokenToString[cur_token.token]);
        if (cur_token.token==NUM) {
            fprintf(tree_fp, "%d ", cur_token.value.num);
        }
        else if (cur_token.token==RNUM) {
            fprintf(tree_fp, "%f ", cur_token.value.rnum);
        }
        else {
            fprintf(tree_fp, "--- ");
        }
        if (treenode->parent == NULL) {

        }
        else {
            grammar_symbol* parent_symbol = treenode->parent->symbol;
            if (strcmp(parent_symbol->name, "$") == 0) {
                fprintf(tree_fp, "ROOT ");
            }
            else {
                fprintf(tree_fp, "%s ", parent_symbol->name);
            }
        }
        fprintf(tree_fp, "YES ");
        grammar_symbol* cur_symbol = treenode->symbol;
        fprintf(tree_fp, "%s\n", cur_symbol->name);
    }
    else {
        struct token_info cur_token = treenode->token;
        fprintf(tree_fp, "--- --- --- --- ");
        if (treenode->parent == NULL) {

        }
        else {
            grammar_symbol* parent_symbol = treenode->parent->symbol;
            if (strcmp(parent_symbol->name, "$") == 0) {
                fprintf(tree_fp, "ROOT ");
            }
            else {
                fprintf(tree_fp, "%s ", parent_symbol->name);
            }
        }
        fprintf(tree_fp, "NO ");
        grammar_symbol* cur_symbol = treenode->symbol;
        fprintf(tree_fp, "%s\n", cur_symbol->name);
    }
}

void printTreeNode(treeNode* treenode, FILE* tree_fp)
{
    if(treenode==NULL) {
        return;
    }
    if(treenode->firstchild!=NULL) {
        printTreeNode(treenode->firstchild, tree_fp);
    }

    //Print Node details
    printNodeInfo (treenode, tree_fp);

    if(treenode->firstchild!=NULL)
    {
        treeNode* restTreeChild=treenode->firstchild->next;
        while(restTreeChild != NULL)
        {
            printTreeNode(restTreeChild, tree_fp);
            restTreeChild=restTreeChild->next;
        }
    }
}

void printParseTree(treeNode* root, char* outputfile)
{
    if(isError) return ;
    FILE* tree_fp = fopen(outputfile, "w+");
    if (tree_fp == NULL) {
        printf("Output file doesn't exist!\n");
        exit(0);
    }
    treeNode * curTreeNode=root;
    fprintf(tree_fp, "Lexeme CurrentNodeLineNo. TokenName ValueIfNumber ParentNodeSymbol IsLeafNode(YES/NO) NodeSymbol\n");
    if (root == NULL) {
        return;
    }
    printTreeNode(root->firstchild, tree_fp);
    fclose(tree_fp);
}

void deallocateFromParser () {
    delete_ht(nonterminal_ht);
    delete_ht(terminal_ht);
    deallocateParseTable();
    deallocateGrammar();
}