#include "ast.h"

const char* const ast_node_id[] = {
    "terminal", // 0
    "program", // 1
    "module_declarations", // 2
    "module_declaration", // 3
    "other_modules", // 4
    "driver_module", // 5
    "module", // 6
    "ret", // 7
    "input_plist", // 8
    "output_plist", // 9
    "datatype", // 10
    "range_arrays", // 11
    "type", // 12
    "module_def", // 13
    "statements",  // 14
    "statement",  // 15
    "io_statement_get",  // 16
    "io_statement_print",  // 17
    "N14",  // 18
    "var_print",  // 19
    "minus_var_print",  // 20
    "P1",  // 21
    "assignment_statement",  // 22
    "lvalue_statement",  // 23
    "lvalueARR_statement",  // 24
    "index_arr",  // 25
    "module_reuse_statement",  // 26
    "actual_para_list",  // 27
    "singular_para_list",  // 28
    "minus_singular_para_list",  // 29
    "N13",  // 30
    "N11",  // 31
    "optional",  // 32
    "id_list",  // 33
    "U",  // 34
    "arithmetic_or_boolean_expression",  // 35
    "any_term",  // 36
    "N8",  // 37
    "arithmetic_expression",  // 38
    "term", // 39
    "factor", // 40
    "element_index_with_expressions", // 41
    "minus_element_index_with_expressions", // 42
    "arr_expression", // 43
    "arr_term", // 44
    "declare_statement", // 45
    "conditional_statement", // 46
    "case_statements", // 47
    "FOR", // 48
    "WHILE", // 49
    "range_for_loop", // 50
    "index_for_loop", // 51
};

Token_Info* createHeapTokenInfo(Token_Info old_token) {
    Token_Info* new_token = (Token_Info*)malloc(sizeof(Token_Info));
    new_token->token = old_token.token;
    strcpy(new_token->lexeme, old_token.lexeme);
    new_token->lineNumber = old_token.lineNumber;
    new_token->value = old_token.value;
    return new_token;
}

Ast_Node* createASTNode() {
    Ast_Node* root = (Ast_Node*)malloc(sizeof(Ast_Node));
    root->type = 0;
    root->syn_next = NULL;
    root->inh_1 = NULL;
    root->token_data = NULL;
    root->child_1 = NULL;
    root->child_2 = NULL;
    root->child_3 = NULL;
    root->child_4 = NULL;
    root->child_5 = NULL;
}

void printASTNode(Ast_Node* root, FILE* f) {
    if(root == NULL) {
        fprintf(f, "NULL");
    } else {
        fprintf(f, "%d %s\n", root->type, ast_node_id[root->type]);
    }
}

void printASTNodeSTDOUT(Ast_Node* root, Ast_Node* prev) {
    if(root == NULL) {
        printf("NULL ");
    } else {
        printf("%d %s ; ", root->type, ast_node_id[root->type]);
    }
    
    printf("Parent: ");
    if(prev == NULL) {
        printf("NULL\n");
    } else {
        printf("%d %s\n", prev->type, ast_node_id[prev->type]);
    }
}

void traverseAST(Ast_Node* root, Ast_Node* prev) {
    printASTNodeSTDOUT(root, prev);
    if(root->child_1 != NULL) {
        traverseAST(root->child_1, root);
    }
    if(root->child_2 != NULL) {
        traverseAST(root->child_2, root);
    }
    if(root->child_3 != NULL) {
        traverseAST(root->child_3, root);
    }
    if(root->child_4 != NULL) {
        traverseAST(root->child_4, root);
    }
    if(root->child_5 != NULL) {
        traverseAST(root->child_5, root);
    }
    if(root->syn_next != NULL) {
        traverseAST(root->syn_next, root);
    }
}

Ast_Node* generateAST(treeNode* curr, Ast_Node* prev) {
    Ast_Node* root = createASTNode();

    bool return_null = false; // For productions like rule 3.
    
    // Handle case where curr == NULL here.

    int rule_no = curr->grammar_rule;

    rule_no++; // Remember one-based indexing!!!!!!!!

    treeNode* temp;

    switch (rule_no)
    {
    case 0:
        // This is the case for tokens.
        root->token_data = createHeapTokenInfo(curr->token);
        break;
    case 1:
        assert(prev == NULL); // i.e this case is only possible at root of program.
        root->type = 1;
        // post-order traversal
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->child_3 = generateAST(curr->firstchild->next->next, NULL);
        root->child_4 = generateAST(curr->firstchild->next->next->next, NULL);
        break;
    case 2:
        root->type = 2;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->firstchild, NULL);
        break;
    case 3:
        return_null = true; // handle case accordingly in the end.
        break;
    case 4:
    // 4. <moduleDeclaration> -> DECLARE MODULE ID SEMICOL
    // Instead of making token, it is much better to just make an Ast_Node for ID.
    // But we will have to handle a case for it. 
        root->type = 3;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        // Token_Info ID = curr->firstchild->next->next->token;
        // root->token_data = createHeapTokenInfo(ID);
        break;
    case 5:
        root->type = 4;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 6:
    // 6. <otherModules> -> @
        return_null = true;
        break;
    case 7:
    //  <driverModule> -> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
        root->type = 5;
        root->token_data = createHeapTokenInfo(curr->firstchild->token); // Needed for symbol table start
        root->child_1 = generateAST(curr->firstchild->next->next->next->next, NULL);
        break;
    case 8:
    // <module> -> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
        root->type = 6;
        root->token_data = createHeapTokenInfo(curr->firstchild->token); // Needed for symbol table start
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next->next->next->next;
        root->child_2 = generateAST(temp, NULL); // input_plist
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // ret
        temp = temp->next;
        root->child_4 = generateAST(temp, NULL); // moduleDef
        break;
    case 9:
    // 9. <ret> -> RETURNS SQBO <output_plist> SQBC SEMICOL
        root->type = 7;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 10:
    // 10. <ret> -> @
        return_null = true;
        break;
    case 11:
    // 11. <input_plist> -> ID COLON <dataType> <N1>
        root->type = 8;
        root->child_1 = generateAST(curr->firstchild, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next, NULL); // dataType
        root->syn_next = generateAST(curr->firstchild->next->next->next, NULL); // N1
        break;
    case 12:
    // 12. <N1_1> -> COMMA ID COLON <dataType> <N1_2>
        root->type = 8; // This is a controversial decision. My thinking is that, for an AST, 
        // N1 and input_plist are exactly the same. 
        root->child_1 = generateAST(curr->firstchild->next, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next->next, NULL); // dataType
        root->syn_next = generateAST(curr->firstchild->next->next->next->next, NULL); // N1 
        break;
    case 13:
    // 13. <N1> -> @
        return_null = true;
        break;
    case 14:
    // 14. <output_plist> -> ID COLON <type> <N2>
        root->type = 9;
        root->child_1 = generateAST(curr->firstchild, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next, NULL); // type
        root->syn_next = generateAST(curr->firstchild->next->next->next, NULL); // N2
        break;
    case 15:
    // 15. <N2_1> -> COMMA ID COLON <type> <N2_2>
        root->type = 9; // See comment on Rule 12.
        root->child_1 = generateAST(curr->firstchild->next, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next->next, NULL); // type
        root->syn_next = generateAST(curr->firstchild->next->next->next->next, NULL); // N2 
        break;
    case 16:
    // 16. <N2> -> @
        return_null = true;
        break;
    case 17:
    // 17. <dataType> -> ARRAY SQBO <range_arrays> SQBC OF <type>
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next->next->next->next, NULL);
        break;
    case 18:
    // 18. <range_arrays> -> <index_arr_1> RANGEOP <index_arr_2>
        root->type = 11;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 19:
    // 19. <dataType> -> INTEGER
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 20:
    // 20. <dataType> -> REAL
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 21:
    // 21. <dataType> -> BOOLEAN
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 22:
    // 22. <type> -> INTEGER
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 23:
    // 23. <type> -> REAL
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 24:
    // 24. <type> -> BOOLEAN
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 25:
    // 25. <moduleDef> -> START <statements> END
        root->type = 13;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->child_3 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 26:
    // 26. <statements> -> <statement><statements>
        root->type = 14;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 27:
    // 27. <statements> -> @
        return_null = true;
        break;
    case 28:
    // 28. <statement> -> <ioStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 29:
    // 29. <statement> -> <simpleStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 30:
    // 30. <statement> -> <declareStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 31:
    // 31. <statement> -> <conditionalStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 32:
    // 32. <statement> -> <iterativeStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 33:
    // 33. <ioStmt> -> GET_VALUE BO ID BC SEMICOL
        root->type = 16;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 34:
    // 34. <ioStmt> -> PRINT BO <var_print> BC SEMICOL
        root->type = 17;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 35:
    // 35. <N14> -> NUM
        root->type = 18;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 36:
    // 36. <N14> -> RNUM
        root->type = 18;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 37:
    // 37. <var_print> -> PLUS <N14>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 38:
    // 38. <var_print> -> MINUS <N14>
        root->type = 20;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 39:
    // 39. <var_print> -> <N14>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 40:
    // 40. <var_print> -> ID <P1>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1); // Inherited attibute
        break;
    case 41:
    // 41. <var_print> -> <boolConstt>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 42:
    // 42. <P1> -> SQBO <index_arr> SQBC
        root->type = 21;
        assert(prev != NULL); // Rule 40.
        root->inh_1 = prev->child_1;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 43:
    // 43. <P1> -> @
        return_null = true;
        break;
    case 44:
    // 44. <simpleStmt> -> <assignmentStmt>
        free(root); // Assure that this works correctly.
        root = generateAST(curr->firstchild, NULL);
        break;
    case 45:
    // 45. <simpleStmt> -> <moduleReuseStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 46:
    // 46. <assignmentStmt> -> ID <whichStmt>
        root->type = 22;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1); // Pass information about ID
        break;
    case 47:
    // 47. <whichStmt> -> <lvalueIDStmt>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 48:
    // 48. <whichStmt> -> <lvalueARRStmt>
        free(root);
        root = generateAST(curr->firstchild, prev); // ID keeps passing downward.
        break;
    case 49:
    // 49. <lvalueIDStmt> -> ASSIGNOP <expression> SEMICOL
        root->type = 23;
        root->child_1 = generateAST(curr->firstchild->next, prev); 
        root->inh_1 = prev;
        break;
    case 50:
    // 50. <lvalueARRStmt> -> SQBO <element_index_with_expressions> SQBC ASSIGNOP <expression> SEMICOL
        root->type = 24;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL);
        root->inh_1 = prev;
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, prev);
        break;
    case 51:
    // 51. <index_arr> -> <sign> <new_index>
        // Sadly, we'll have to create a node for sign here. I am not capable enough to 
        // interpret token_info and make this similar to Rule 37. Basically, just forget
        // that rule 37 and 51 COULD HAVE been similar, and keep solving them independently.
        root->type = 25;
        root->child_1 = generateAST(curr->firstchild, NULL); 
        root->child_2 = generateAST(curr->firstchild->next, NULL); 
        break;
    case 52:
    // 52. <new_index> -> NUM
        // Compress the tree! Just return NUM itself. No need for a seperate token for <new_index>.
        // Also optimize this in previous rules!!!!
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 53:
    // 53. <new_index> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 54:
    // 54. <sign> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 55:
    // 55. <sign> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 56:
    // 56. <sign> -> @
        // FIX!!!! BEST THING TO DO IS RETURN TOKEN_INFO OF FIRST. 
        // Currently : if NULL is returned, it means PLUS.
        return_null = true;
        break;
    case 57:
    // 57. <moduleReuseStmt> -> <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
        root->type = 26;
        temp = curr->firstchild; 
        root->child_1 = generateAST(temp, NULL); // optional
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, NULL); // ID
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // actual_para_list
        break;
    case 58:
    // 58. <actual_para_list> -> <singular_para_list> <N12>
        root->type = 27;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 59:
    // 59. <N12> -> COMMA <singular_para_list> <N12>
        root->type = 27;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 60:
    // 60. <N12> -> @
        return_null = true;
        break;
    case 61:
    // 61. <singular_para_list> -> <boolConstt>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 62:
    // 62. <singular_para_list> -> PLUS <N13>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 63:
    // 63. <singular_para_list> -> MINUS <N13>
        root->type = 29;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 64:
    // 64. <singular_para_list> -> <N13>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 65:
    // 65. <N13> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 66:
    // 66. <N13> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 67:
    // 67. <N13> -> ID <N11>
        root->type = 30;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        // if child_2 comes out to be NULL, it means rule 69 was applied.
        break;
    case 68:
    // 68. <N11> -> SQBO <element_index_with_expressions> SQBC
        // Bound checking happens HERE.
        root->type = 31;
        assert(prev != NULL);
        root->inh_1 = prev;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 69:
    // 69. <N11> -> @
        return_null = true;
        break;
    case 70:
    // 70. <optional> -> SQBO <idList> SQBC ASSIGNOP
        root->type = 32;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 71:
    // 71. <optional> -> @
        return_null = true;
        break;
    case 72:
    // 72. <idList> -> ID <N3>
        root->type = 33;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Also see Rule. 128!!!
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next, prev);
        break;
    case 73:
    // 73. <N3> -> COMMA ID <N3>
        root->type = 33;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next->next, prev);
        break;
    case 74:
    // 74. <N3> -> @
        return_null = true;
        break;
    case 75:
    // 75. <expression> -> <arithmeticOrBooleanExp>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 76:
    // 76. <expression> -> <U>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 77:
    // 77. <U> -> <op1> <new_NT>
        root->type = 34;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, prev);
        break;
    case 78:
    // 78. <new_NT> -> BO <arithmeticExpr> BC
        free(root);
        root = generateAST(curr->firstchild, prev);
        break;
    case 79:
    // 79. <new_NT> -> <var_id_num>
        free(root);
        root = generateAST(curr->firstchild, prev);
        break;
    case 80:
    // 80. <arithmeticOrBooleanExp> -> <AnyTerm> <N7>
        root->type = 35;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 81:
    // 81. <N7> -> <logicalOp> <AnyTerm> <N7>
        root->type = 35;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL); // Type checking for this term and prev term can happen here???
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 82:
    // 82. <N7> -> @
        return_null = true;
        break;
    case 83:
    // 83. <AnyTerm> -> <arithmeticExpr> <N8>
        root->type = 36;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 84:
    // 84. <boolConstt> -> TRUE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 85:
    // 85. <boolConstt> -> FALSE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 86:
    // 86. <N8> -> <relationalOp> <arithmeticExpr>
        root->type = 37;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        // Type checking here.
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        break;
    case 87:
    // 87. <N8> -> @
        return_null = true;
        break;
    case 88:
    // 88. <arithmeticExpr> -> <term> <N4>
        root->type = 38;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 89:
    // 89. <N4> -> <op1> <term> <N4>
        /*
        How to distinguish between rules 88 and 89?
        Ans. Using inh_1. It will be NULL for rule 88, but not for 89.
        Also, number of children are different.
        */
        root->type = 38;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Type checking happens here??
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 90:
    // 90. <N4> -> @
        return_null = true;
        break;
    case 91:
    // 91. <term> -> <factor> <N5>
        root->type = 39;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 92:
    // 92. <N5> -> <op2> <factor> <N5>
        root->type = 39;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Type checking happens here??
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 93:
    // 93. <N5> -> @
        return_null = true;
        break;
    case 94:
    // 94. <factor> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 95:
    // 95. <factor> -> <boolConstt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 96:
    // 96. <factor> -> ID <N11>
        root->type = 40;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 97:
    // 97. <factor> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 98:
    // 98. <factor> -> BO <arithmeticOrBooleanExp> BC
        free(root);
        root = generateAST(curr->firstchild->next, NULL);
        break;
    case 99:
    // 99. <array_element> -> ID SQBO <element_index_with_expressions> SQBC
        // Rule is useless. Never comes on right side. Ignore it.
        assert(false);
        return_null = true;
        break;
    case 100:
    // 100. <element_index_with_expressions> -> <arrExpr>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 101:
    // 101. <element_index_with_expressions> -> PLUS <new_index>
        root->type = 41;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 102:
    // 102. <element_index_with_expressions> -> MINUS <new_index>
        root->type = 42;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 103:
    // 103. <arrExpr> -> <arrTerm> <arr_N4> 
        // Same as rule 91
        root->type = 43;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 104:
    // 104. <arr_N4> -> <op1> <arrTerm> <arr_N4>
        // Same as rule 92
        root->type = 43;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 105:
    // 105. <arr_N4> -> @
        return_null = true;
        break;
    case 106:
    // 106. <arrTerm> -> <arrFactor> <arr_N5>
        // Same as rule 103
        root->type = 44;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 107:
    // 107. <arr_N5> -> <op2> <arrFactor> <arr_N5>
        // Same as rule 104
        root->type = 44;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 108:
    // 108. <arr_N5> -> @
        return_null = true;
        break;
    case 109:
    // 109. <arrFactor> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 110:
    // 110. <arrFactor> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 111:
    // 111. <arrFactor> -> <boolConstt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 112:
    // 112. <arrFactor> -> BO <arrExpr> BC
        free(root);
        root = generateAST(curr->firstchild->next, NULL);
        break;
    case 113:
    // 113. <var_id_num> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 114:
    // 114. <var_id_num> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 115:
    // 115. <var_id_num> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 116:
    // 116. <op1> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 117:
    // 117. <op1> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 118:
    // 118. <op2> -> MUL
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 119:
    // 119. <op2> -> DIV
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 120:
    // 120. <logicalOp> -> AND
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 121:
    // 121. <logicalOp> -> OR
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 122:
    // 122. <relationalOp> -> LT
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 123:
    // 123. <relationalOp> -> LE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 124:
    // 124. <relationalOp> -> GT
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 125:
    // 125. <relationalOp> -> GE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 126:
    // 126. <relationalOp> -> EQ
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 127:
    // 127. <relationalOp> -> NE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 128:
    // 128. <declareStmt> -> DECLARE <idList> COLON <dataType> SEMICOL
        root->type = 45;
        // Take care!! Child order is reversed.
        root->child_1 = generateAST(curr->firstchild->next->next->next, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 129:
    // 129. <conditionalStmt> -> SWITCH BO ID BC START <caseStmts> <default> END
        root->type = 46;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // START
        temp = temp->next;
        root->child_3 = generateAST(temp, NULL); // caseStmts
        temp = temp->next;
        root->child_4 = generateAST(temp, NULL); // default
        temp = temp->next;
        root->child_5 = generateAST(temp, NULL); // END
        break;
    case 130:
    // 130. <caseStmts> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>
        root->type = 47;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL); // value
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // statements
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // N9
        break;
    case 131:
    // 131. <N9> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>
        root->type = 47;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL); // value
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // statements
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // N9
        break;
    case 132:
    // 132. <N9> -> @
        return_null = true;
        break;
    case 133:
    // 133. <value> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 134:
    // 134. <value> -> TRUE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 135:
    // 135. <value> -> FALSE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 136:
    // 136. <default> -> DEFAULT COLON <statements> BREAK SEMICOL
        free(root);
        root = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 137:
    // 137. <default> -> @
        return_null = true;
        break;
    case 138:
    // 138. <iterativeStmt> -> FOR BO ID IN <range_for_loop> BC START <statements> END
        root->type = 48;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // range_for_loop
        temp = temp->next->next;
        root->child_3 = generateAST(temp, NULL); // START
        temp = temp->next;
        root->child_4 = generateAST(temp, NULL); // statements
        temp = temp->next;
        root->child_5 = generateAST(temp, NULL); // END
        break;
    case 139:
    // 139. <iterativeStmt> -> WHILE BO <arithmeticOrBooleanExp> BC START <statements> END
        root->type = 49;
        root->token_data = createHeapTokenInfo(curr->firstchild->token);
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // arithmeticOrBooleanExp
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // START
        temp = temp->next;
        root->child_3 = generateAST(temp, NULL); // statements
        temp = temp->next;
        root->child_4 = generateAST(temp, NULL); // END
        break;
    case 140:
    // 140. <range_for_loop> -> <index_for_loop> RANGEOP <index_for_loop>
        root->type = 50;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 141:
    // 141. <index_for_loop> -> <sign_for_loop> <new_index_for_loop>
        root->type = 51;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        break;
    case 142:
    // 142. <new_index_for_loop> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 143:
    // 143. <sign_for_loop> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 144:
    // 144. <sign_for_loop> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 145:
    // 145. <sign_for_loop> -> @
        return_null = true;
        break;
    default:
        assert(false);
    }

    // printASTNode(root);

    // Remember to FREE treenodes!!!
    free(curr);
    if(return_null) {
        free(root);
        return NULL;
    }
    return root;
}
