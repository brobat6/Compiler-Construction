#include <stdio.h>
#include "codegen.h"

// rbp, rbx, r12, r13, r14, r15 -- DO NOT USE
// All others free to use

////////////////// create a label ArrayOutOfBoundsError

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// checkArrayBounds and setArrayBounds saath saath karne hain

//////////////////// arrays are pass by reference, so whatever changes are made in function should be made to the original array too
// lol
FILE* fp;
storageStructure st;
int cur_offset;
int comp_label = 0;

void initialiseStorage () {
    st.capacity = 10;
    st.size = 0;
    st.storage = calloc(10, sizeof(STEntry));
}

STEntry getNewTemporary (Type dataType) {
    STEntry cur;
    cur.type = dataType;
    cur.offset = cur_offset;
    if (dataType == TYPE_BOOLEAN) {
        cur_offset+=1;
        cur.width = 1;
    }
    else if (dataType == TYPE_INTEGER) {
        cur_offset+=2;
        cur.width = 2;
    }
    else if (dataType == TYPE_REAL) {
        cur_offset+=4;
        cur.width = 4;
    }
    return cur;
}

void codeGenASTTraversal (Ast_Node* cur_ast_node);

void checkBounds (Ast_Node* cur_ast_node) {
    // if cur id is an array and its bounds are variable, then find their value
    // so store the array dynamically, set offset etc.
}

void printDataSection () {
    fprintf(fp, "\tsection .bss\n");
    fprintf(fp, "buffer:  resb  10000\n");
    fprintf(fp, "\tsection .data\n");
    fprintf(fp, "inputInt:  db  '%%hd', 0\n");
    fprintf(fp, "inputFloat:  db  '%%f', 0\n");
    fprintf(fp, "inputBool:  db  '%%hhu', 0\n");
    fprintf(fp, "outputInt:  db  '%%hd', 10, 0\n");
    fprintf(fp, "outputFloat:  db  '%%f', 10, 0\n");
    fprintf(fp, "outputTrue:  db  'true', 0\n");
    fprintf(fp, "outputFalse:  db  'false', 0\n");
    // out
}

void getValue (Ast_Node* cur_ast_node) {
    // take input num, rnum, bool, array in ids - A[k] not possible
    // scanf etc
    STEntry* cur_node = recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->token_data);
    int offset_req = cur_node->offset;
    fprintf(fp, "\txor eax, eax\n");
    if (cur_node->isArray) {
        // to do after dynamic arrays ka sort ho jaaye //////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    else {
        if (cur_node->type == TYPE_BOOLEAN) {
            fprintf(fp, "\tmov rdi, inputBool\n");
            fprintf(fp, "\tmov rsi, buffer + %d\n", offset_req);
        }
        else if (cur_node->type == TYPE_INTEGER) {
            fprintf(fp, "\tmov rdi, inputInt\n");
            fprintf(fp, "\tmov rsi, buffer + %d\n", offset_req);
        }
        else if (cur_node->type == TYPE_REAL) {

        }
        fprintf(fp, "\tcall scanf\n");
    }
}

void storeAtOffset (int offset, Type dataType) {
    if (dataType == TYPE_INTEGER) {
        fprintf(fp, "\tmov [buffer + %d], ax\n", offset);
    }
    else if (dataType == TYPE_BOOLEAN) {
        fprintf(fp, "\tmov [buffer + %d], al\n", offset);
    }
    else if (dataType == TYPE_REAL) {

    }
}

void printHelper (STEntry cur_entry) { ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    fprintf(fp, "\txor eax, eax\n");
    if (cur_entry.type == TYPE_BOOLEAN) {
        fprintf(fp, "\tmov rdi, outputTrue\n");
        fprintf(fp, "\tmov al, [buffer + %d]\n", cur_entry.offset);
        fprintf(fp, "\tcmp al, 0\n");
        fprintf(fp, "\tjne comp_label%d\n", comp_label);
        fprintf(fp, "\tmov rdi, outputFalse\n");
        fprintf(fp, "comp_label%d:\n", comp_label);
        fprintf(fp, "\txor eax, eax\n");
        fprintf(fp, "\tcall puts\n");
        comp_label++;
    }
    else if (cur_entry.type == TYPE_INTEGER) {
        fprintf(fp, "\tmov rdi, outputInt\n");
        fprintf(fp, "\tmov rax, 0\n");
        fprintf(fp, "\tmov ax, [buffer + %d]\n", cur_entry.offset);
        fprintf(fp, "\tmov rsi, rax\n");
        fprintf(fp, "\tcall printf\n");
    }
    else if (cur_entry.type == TYPE_REAL) {
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        fprintf(fp, "\tcall printf\n");
    }
}

void printValue (Ast_Node* cur_ast_node) { //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // output num, rnum, bool in ids or constants - A[k] possible
    // output whole array
    // call arithemetic function to calculate the value and then print it
    // above can't be done
    if (cur_ast_node->type == 19) {/////////////////////////// problem is that <P1> could be null
        if (cur_ast_node->child_2 == NULL) {
            if (cur_ast_node->child_1->token_data->token == TRUE) {
                fprintf(fp, "\tmov rdi, outputTrue\n");
                fprintf(fp, "\tcall puts\n");
            }
            else if (cur_ast_node->child_1->token_data->token == FALSE) {
                fprintf(fp, "\tmov rdi, outputFalse\n");
                fprintf(fp, "\tcall puts\n");
            }
            else if (cur_ast_node->child_1->token_data->token == ID) {
                STEntry* cur_id = recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
                printHelper(*cur_id);
            }
            else {
                STEntry cur_temp;
                if (cur_ast_node->child_1->token_data->token == NUM) {
                    cur_temp = getNewTemporary(TYPE_INTEGER);
                    fprintf(fp, "\tmov ax, %d\n", cur_ast_node->child_1->token_data->value.num);
                    storeAtOffset(cur_temp.offset, TYPE_INTEGER);
                    printHelper(cur_temp);
                }
                else {
                    cur_temp = getNewTemporary(TYPE_REAL);
                    storeAtOffset(cur_temp.offset, TYPE_REAL);/////////////////////////////////////////////////////////////
                }
            }
            printValue(cur_ast_node->child_1);
        }
        else { // id p1, p1 not null
            STEntry* cur_id = recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
            if (cur_id->isArray) {

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            }
        }
    }
    else if (cur_ast_node->type == 20) {
        STEntry cur_temp;
        cur_ast_node = cur_ast_node->child_1;
        if (cur_ast_node->child_1->token_data->token == NUM) {
            cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, %d\n", -cur_ast_node->child_1->token_data->value.num);
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            printHelper(cur_temp);
        }
        else {
            cur_temp = getNewTemporary(TYPE_REAL);
            storeAtOffset(cur_temp.offset, TYPE_REAL);/////////////////////////////////////////////////////////////
        }
    }
}

STEntry evaluateArrayExpression (Ast_Node* cur_ast_node) {
    if (cur_ast_node->type == 0) {
        if (cur_ast_node->token_data->token == NUM) {
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, %d\n", cur_ast_node->token_data->value.num);
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else if (cur_ast_node->token_data->token == ID) {
            return *recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->token_data);
        }
        else if (cur_ast_node->token_data->token == TRUE) {
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov al, 1\n");
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
        else if (cur_ast_node->token_data->token == FALSE) {
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov al, 0\n");
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
    }
    else if (cur_ast_node->type == 43 && cur_ast_node->child_2 == NULL) {
        if (cur_ast_node->syn_next) { // N4 is not null
            STEntry node_1 = evaluateArrayExpression(cur_ast_node->child_1); // term
            STEntry node_2 = evaluateArrayExpression(cur_ast_node->syn_next); // n4
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == PLUS) {
                fprintf(fp, "\tadd ax, bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == MINUS) {
                fprintf(fp, "\tsub ax, bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N4 is null
            return evaluateArrayExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 43 && cur_ast_node->child_2 != NULL) {
        if (cur_ast_node->syn_next) { // N4 is not null
            STEntry node_1 = evaluateArrayExpression(cur_ast_node->child_2);
            STEntry node_2 = evaluateArrayExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == PLUS) {
                fprintf(fp, "\tadd ax, bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == MINUS) {
                fprintf(fp, "\tsub ax, bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N4 is null
            return evaluateArrayExpression(cur_ast_node->child_2);
        }
    }
    else if (cur_ast_node->type == 44 && cur_ast_node->child_2 == NULL) {
        if (cur_ast_node->syn_next) { // N5 is not null
            STEntry node_1 = evaluateArrayExpression(cur_ast_node->child_1); // term
            STEntry node_2 = evaluateArrayExpression(cur_ast_node->syn_next); // n4
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == MUL) {
                fprintf(fp, "\timul bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == DIV) {
                fprintf(fp, "\tcwd\n");
                fprintf(fp, "\tidiv bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N5 is null
            return evaluateArrayExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 44 && cur_ast_node->child_2 != NULL) {
        if (cur_ast_node->syn_next) { // N5 is not null
            STEntry node_1 = evaluateArrayExpression(cur_ast_node->child_2);
            STEntry node_2 = evaluateArrayExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == MUL) {
                fprintf(fp, "\timul bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == DIV) {
                fprintf(fp, "\tcwd\n");
                fprintf(fp, "\tidiv bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N5 is null
            return evaluateArrayExpression(cur_ast_node->child_2);
        }
    }
}

void outOfBoundsHelper (STEntry* cur_id, STEntry cur_node) { // cur id is id, and cur node contains arr index
    int l_index = cur_id->range.lower.value; //////////////////////////////////////////////////////////////////////////////////////////////////////
    int r_index = cur_id->range.upper.value;
    fprintf(fp, "\tmov ax, [buffer + %d]\n", cur_node.offset);
    fprintf(fp, "\tmov bx, %d\n", l_index);
    fprintf(fp, "\tcmp ax, bx\n");
    fprintf(fp, "\tjl ArrayOutOfBoundsError\n");
    fprintf(fp, "\tmov bx, %d\n", r_index);
    fprintf(fp, "\tcmp ax, bx\n");
    fprintf(fp, "\tjg ArrayOutOfBoundsError\n");
    fprintf(fp, "\tmov bx, %d\n", l_index);
    fprintf(fp, "\tsub ax, bx\n"); // should be >= 0
    fprintf(fp, "\tmov bx, %d\n", cur_id->width);
    fprintf(fp, "\tmul bx\n");
    fprintf(fp, "\tmov rbx, 0");
    fprintf(fp, "\tmov bx, ax\n");
    fprintf(fp, "\tmov dx, %d\n", cur_id->offset);
    fprintf(fp, "\tadd bx, dx\n");
}

STEntry outOfBoundsCheckArrIndWithExpr (STEntry* cur_id, Ast_Node* cur_ast_node) { /////////////////// check ----------------------------------
    STEntry cur_node;
    if (cur_ast_node->type == 41) {
        if (cur_ast_node->child_1->token_data->token == NUM) {
            cur_node = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, %d\n", cur_ast_node->child_1->token_data->token);
            storeAtOffset(cur_node.offset, TYPE_INTEGER);
        }
        else if (cur_ast_node->child_1->token_data->token == ID) {
            cur_node = *recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
        }
    }
    else if (cur_ast_node->type == 42) {
        if (cur_ast_node->child_1->token_data->token == NUM) {
            cur_node = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov bx, %d\n", cur_ast_node->child_1->token_data->value.num);
            fprintf(fp, "\tmov ax, 0\n");
            fprintf(fp, "\tsub ax, bx\n");
            storeAtOffset(cur_node.offset, TYPE_INTEGER);
        }
        else if (cur_ast_node->child_1->token_data->token == ID) {
            STEntry temp = *recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
            cur_node = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", temp.offset);
            fprintf(fp, "\tmov ax, 0\n");
            fprintf(fp, "\tsub ax, bx\n");
            storeAtOffset(cur_node.offset, TYPE_INTEGER);
        }
    }
    else if (cur_ast_node->type == 43) {
        cur_node = evaluateArrayExpression(cur_ast_node);
    }
    STEntry new_node = getNewTemporary(cur_id->type);
    outOfBoundsHelper(cur_id, cur_node);
    if (cur_id->type == TYPE_BOOLEAN) {
        fprintf(fp, "\tmov al, [buffer + rbx]\n");
        storeAtOffset(new_node.offset, TYPE_BOOLEAN);
    }
    else if (cur_id->type == TYPE_INTEGER) {
        fprintf(fp, "\tmov ax, [buffer + rbx]\n");
        storeAtOffset(new_node.offset, TYPE_INTEGER);
    }
    else if (cur_id->type == TYPE_REAL) {
        fprintf(fp, "\tmov eax, [buffer + rbx]\n");//////////////////////////////////////////////////////////////////////////////////////////////////////
        storeAtOffset(new_node.offset, TYPE_REAL);
    }
    return new_node;
}

STEntry evaluateExpression (Ast_Node* cur_ast_node) {
    if (cur_ast_node->type == 0) {
        if (cur_ast_node->token_data->token == NUM) {
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, %d\n", cur_ast_node->token_data->value.num);
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else if (cur_ast_node->token_data->token == RNUM) {



                // cur_ast_node->token_data->value.rnum



        }
        else if (cur_ast_node->token_data->token == ID) {
            return *recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->token_data);
        }
        else if (cur_ast_node->token_data->token == TRUE) {
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov al, 1\n");
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
        else if (cur_ast_node->token_data->token == FALSE) {
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov al, 0\n");
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
    }
    else if (cur_ast_node->type == 34 && cur_ast_node->child_1->token_data->token == MINUS) {
        if (cur_ast_node->child_2->type == 38) {
            STEntry cur_node = evaluateExpression(cur_ast_node->child_2);
            if (cur_node.type == TYPE_INTEGER) {
                STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov bx, [buffer + %d]\n", cur_node.offset);
                fprintf(fp, "\tmov ax, 0\n");
                fprintf(fp, "\tsub ax, bx\n");
                storeAtOffset(cur_temp.offset, TYPE_INTEGER);
                return cur_temp;
            }
            else if (cur_node.type == TYPE_REAL) {

            }
        }
        else { // var_id_num
            if (cur_ast_node->child_2->token_data->token == NUM) {
                STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov bx, %d\n", cur_ast_node->child_2->token_data->value.num);
                fprintf(fp, "\tmov ax, 0\n");
                fprintf(fp, "\tsub ax, bx\n");
                storeAtOffset(cur_temp.offset, TYPE_INTEGER);
                return cur_temp;
            }
            else if (cur_ast_node->child_2->token_data->token == RNUM) {



                // cur_ast_node->child_2->token_data->value.rnum



            }
            else if (cur_ast_node->child_2->token_data->token == ID) {
                STEntry* cur_node = recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->child_2->token_data);
                if (cur_node->type == TYPE_INTEGER) {
                    STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
                    fprintf(fp, "\tmov bx, [buffer + %d]\n", cur_node->offset);
                    fprintf(fp, "\tmov ax, 0\n");
                    fprintf(fp, "\tsub ax, bx\n");
                    storeAtOffset(cur_temp.offset, TYPE_INTEGER);
                    return cur_temp;
                }
                else if (cur_node->type == TYPE_REAL) {



                    // dddddddddddddd



                }
            }
        }
    }
    else if (cur_ast_node->type == 34 && cur_ast_node->child_1->token_data->token == PLUS) {
        if (cur_ast_node->child_2->type == 38) {
            return evaluateExpression(cur_ast_node->child_2);
        }
        else { // var_id_num
            if (cur_ast_node->child_2->token_data->token == NUM) {
                STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov ax, %d\n", cur_ast_node->child_2->token_data->value.num);
                storeAtOffset(cur_temp.offset, TYPE_INTEGER);
                return cur_temp;
            }
            else if (cur_ast_node->child_2->token_data->token == RNUM) {



                // cur_ast_node->child_2->token_data->value.rnum



            }
            else if (cur_ast_node->child_2->token_data->token == ID) {
                return *recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->child_2->token_data);
            }
        }
    }
    else if (cur_ast_node->type == 35 && cur_ast_node->child_2 == NULL) {
        if (cur_ast_node->syn_next) { // N7 is not null
            STEntry node_1 = evaluateExpression(cur_ast_node->child_1);
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov bl, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov al, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == AND) {
                fprintf(fp, "\tand al, bl\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == OR) {
                fprintf(fp, "\tor al, bl\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
        else { // N7 is null
            return evaluateExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 35 && cur_ast_node->child_2 != NULL) {
        if (cur_ast_node->syn_next) { // N7 is not null
            STEntry node_1 = evaluateExpression(cur_ast_node->child_2);
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov bl, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov al, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == AND) {
                fprintf(fp, "\tand al, bl\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == OR) {
                fprintf(fp, "\tor al, bl\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            return cur_temp;
        }
        else { // N7 is null
            return evaluateExpression(cur_ast_node->child_2);
        }
    }
    else if (cur_ast_node->type == 36) {
        if (cur_ast_node->child_2) { // N8 is not null



            // 2 cases, one if node_1 and node_2 are integers and other if they are real



            STEntry node_1 = evaluateExpression(cur_ast_node->child_1);
            STEntry node_2 = evaluateExpression(cur_ast_node->child_2);
            STEntry cur_temp = getNewTemporary(TYPE_BOOLEAN);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov cx, [buffer + %d]\n", node_2.offset);
            fprintf(fp, "\tcmp bx, cx\n");
            if (cur_ast_node->child_2->child_1->token_data->token == LT || cur_ast_node->child_2->child_1->token_data->token == GE) {
                if (cur_ast_node->child_2->child_1->token_data->token == LT) {
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "\tjge comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
                else {
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "\tjge comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
            }
            else if (cur_ast_node->child_2->child_1->token_data->token == LE || cur_ast_node->child_2->child_1->token_data->token == GT) {
                if (cur_ast_node->child_2->child_1->token_data->token == LE) {
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "\tjg comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
                else {
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "\tjg comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
            }
            else if (cur_ast_node->child_2->child_1->token_data->token == EQ || cur_ast_node->child_2->child_1->token_data->token == NE) {
                if (cur_ast_node->child_2->child_1->token_data->token == EQ) {
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "\tjne comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
                else {
                    fprintf(fp, "\tmov al, 1\n");
                    fprintf(fp, "\tjne comp_label%d\n", comp_label);
                    fprintf(fp, "\tmov al, 0\n");
                    fprintf(fp, "comp_label%d:\n", comp_label);
                }
            }
            storeAtOffset(cur_temp.offset, TYPE_BOOLEAN);
            comp_label++;
            return cur_temp;
        }
        else { // N8 is null
            return evaluateExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 37) {
        return evaluateExpression(cur_ast_node->child_2);
    }
    else if (cur_ast_node->type == 38 && cur_ast_node->child_2 == NULL) {
        if (cur_ast_node->syn_next) { // N4 is not null



            // 2 cases, one if node_1 and node_2 are integers and other if they are real



            STEntry node_1 = evaluateExpression(cur_ast_node->child_1); // term
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next); // n4
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == PLUS) {
                fprintf(fp, "\tadd ax, bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == MINUS) {
                fprintf(fp, "\tsub ax, bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N4 is null
            return evaluateExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 38 && cur_ast_node->child_2 != NULL) {
        if (cur_ast_node->syn_next) { // N4 is not null



            // 2 cases, one if node_1 and node_2 are integers and other if they are real



        
            STEntry node_1 = evaluateExpression(cur_ast_node->child_2);
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == PLUS) {
                fprintf(fp, "\tadd ax, bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == MINUS) {
                fprintf(fp, "\tsub ax, bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N4 is null
            return evaluateExpression(cur_ast_node->child_2);
        }
    }
    else if (cur_ast_node->type == 39 && cur_ast_node->child_2 == NULL) {
        if (cur_ast_node->syn_next) { // N5 is not null



            // 2 cases, one if node_1 and node_2 are integers and other if they are real



            STEntry node_1 = evaluateExpression(cur_ast_node->child_1); // term
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next); // n4
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == MUL) {
                fprintf(fp, "\timul bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == DIV) {
                fprintf(fp, "\tcwd\n");
                fprintf(fp, "\tidiv bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N5 is null
            return evaluateExpression(cur_ast_node->child_1);
        }
    }
    else if (cur_ast_node->type == 39 && cur_ast_node->child_2 != NULL) {
        if (cur_ast_node->syn_next) { // N5 is not null



            // 2 cases, one if node_1 and node_2 are integers and other if they are real



        
            STEntry node_1 = evaluateExpression(cur_ast_node->child_2);
            STEntry node_2 = evaluateExpression(cur_ast_node->syn_next);
            STEntry cur_temp = getNewTemporary(TYPE_INTEGER);
            fprintf(fp, "\tmov ax, [buffer + %d]\n", node_1.offset);
            fprintf(fp, "\tmov bx, [buffer + %d]\n", node_2.offset);
            if (cur_ast_node->syn_next->child_1->token_data->token == MUL) {
                fprintf(fp, "\timul bx\n");
            }
            else if (cur_ast_node->syn_next->child_1->token_data->token == DIV) {
                fprintf(fp, "\tcwd\n");
                fprintf(fp, "\tidiv bx\n");
            }
            storeAtOffset(cur_temp.offset, TYPE_INTEGER);
            return cur_temp;
        }
        else { // N5 is null
            return evaluateExpression(cur_ast_node->child_2);
        }
    }
    else if (cur_ast_node->type == 40) {
        if (!cur_ast_node->child_2) { // n11 gives epsilon
            return *recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
        }
        else { // Out of bounds checking
            return outOfBoundsCheckArrIndWithExpr(recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data), cur_ast_node->child_2);
        }
    }
}

void assignStatement (Ast_Node* cur_ast_node) {
    STEntry* l_entry = recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
    cur_ast_node = cur_ast_node->child_2;
    if (cur_ast_node->type == 24) { // A[expr] = expr
        STEntry array_index;
        if (cur_ast_node->child_1->type == 43) {
            array_index = evaluateArrayExpression(cur_ast_node->child_1);
        }
        else if (cur_ast_node->child_1->type == 41) {
            if (cur_ast_node->child_1->child_1->token_data->token == NUM) {
                array_index = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov ax, %d\n", cur_ast_node->child_1->child_1->token_data->value.num);
                storeAtOffset(array_index.offset, TYPE_INTEGER);
            }
            else if (cur_ast_node->child_1->child_1->token_data->token == ID) {
                array_index = *recursiveCheckID(cur_ast_node->child_1->child_1->symbol_table, cur_ast_node->child_1->child_1->token_data);
            }
        }
        else if (cur_ast_node->child_1->type == 42) {
            if (cur_ast_node->child_1->child_1->token_data->token == NUM) {
                array_index = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov bx, %d\n", cur_ast_node->child_1->child_1->token_data->value.num);
                fprintf(fp, "\tmov ax, 0\n");
                fprintf(fp, "\tsub ax, bx\n");
                storeAtOffset(array_index.offset, TYPE_INTEGER);
            }
            else if (cur_ast_node->child_1->child_1->token_data->token == ID) {
                STEntry cur_array_index = *recursiveCheckID(cur_ast_node->child_1->child_1->symbol_table, cur_ast_node->child_1->child_1->token_data);
                array_index = getNewTemporary(TYPE_INTEGER);
                fprintf(fp, "\tmov bx, [buffer + %d]\n", cur_array_index.offset);
                fprintf(fp, "\tmov ax, 0\n");
                fprintf(fp, "\tsub ax, bx\n");
                storeAtOffset(array_index.offset, TYPE_INTEGER);
            }
        }
        STEntry r_value = evaluateExpression(cur_ast_node->child_2);
        outOfBoundsHelper(l_entry, array_index);
        if (l_entry->type == TYPE_BOOLEAN) {
            fprintf(fp, "\tmov al, [buffer + %d]\n", r_value.offset);
            fprintf(fp, "\tmov [buffer + rbx], al\n");
        }
        else if (l_entry->type == TYPE_INTEGER) {
            fprintf(fp, "\tmov ax, [buffer + %d]\n", r_value.offset);
            fprintf(fp, "\tmov [buffer + rbx], ax\n");
        }
        else if (l_entry->type == TYPE_REAL) {
            fprintf(fp, "\tmov eax, [buffer + bx]\n");//////////////////////////////////////////////////////////////////////////////////////
            fprintf(fp, "\tmov [buffer + rbx], eax\n");
        }
    }
    else if (cur_ast_node->type == 23) {
        if (l_entry->isArray) {
            // A = B for array /////////////////////////////////////////////////////////////////////////////////////////////////////
        }
        else {
            STEntry r_value = evaluateExpression(cur_ast_node->child_1);
            fprintf(fp, "\tmov rbx, %d\n", l_entry->offset);
            if (l_entry->type == TYPE_BOOLEAN) {
                fprintf(fp, "\tmov al, [buffer + %d]\n", r_value.offset);
                fprintf(fp, "\tmov [buffer + rbx], al\n");
            }
            else if (l_entry->type == TYPE_INTEGER) {
                fprintf(fp, "\tmov ax, [buffer + %d]\n", r_value.offset);
                fprintf(fp, "\tmov [buffer + rbx], ax\n");
            }
            else if (l_entry->type == TYPE_REAL) {
                fprintf(fp, "\tmov eax, [buffer + rbx]\n");//////////////////////////////////////////////////////////////////////////////////////
                fprintf(fp, "\tmov [buffer + rbx], eax\n");
            }
        }
    }
}

// isime check karna hai ki out of bounds to nahi hai
void setArrayBounds (Ast_Node* cur_ast_node) { /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    STEntry* cur_entry = recursiveCheckID(cur_ast_node->symbol_table, cur_ast_node->token_data);
    if (!cur_entry->isArray) return;
    if (cur_entry->isDynamic.lower) { // lower bound is dynamic
        // cur_entry->range.lower.lexeme
    }
    else {

    }
    if (cur_entry->isDynamic.upper) { // lower bound is dynamic

    }
    else {
        
    }
}

void switchCaseStatements (Ast_Node* cur_ast_node) {
    STEntry* cur_id = recursiveCheckID(cur_ast_node->child_1->symbol_table, cur_ast_node->child_1->token_data);
    if (cur_id->type == TYPE_INTEGER) {
        fprintf(fp, "\tmov ax, [buffer + %d]\n", cur_id->offset);
        Ast_Node* cur_case_stmt = cur_ast_node->child_3;
        int fin_label = comp_label;
        comp_label++;
        while (cur_case_stmt) {
            fprintf(fp, "\tmov bx, %d\n", cur_case_stmt->child_1->token_data->value.num);
            fprintf(fp, "\tcmp ax, bx\n");
            fprintf(fp, "\tjne comp_label%d\n", comp_label);
            codeGenASTTraversal(cur_case_stmt->child_2);
            fprintf(fp, "\tjmp comp_label%d\n", fin_label);
            fprintf(fp, "comp_label%d:\n", comp_label);
            comp_label++;
            cur_case_stmt = cur_case_stmt->child_3;
        }
        // default
        codeGenASTTraversal(cur_ast_node->child_4->child_1);
        fprintf(fp, "comp_label%d:\n", fin_label);
    }
    else if (cur_id->type == TYPE_BOOLEAN) {
        fprintf(fp, "\tmov al, [buffer + %d]\n", cur_id->offset);
        Ast_Node* cur_case_stmt = cur_ast_node->child_3;
        int fin_label = comp_label;
        comp_label++;
        while (cur_case_stmt) {
            if (cur_case_stmt->child_1->token_data->value.b == true) {
                fprintf(fp, "\tmov bl, 1\n");
            }
            else {
                fprintf(fp, "\tmov bx, 0\n");
            }
            fprintf(fp, "\tcmp al, bl\n");
            fprintf(fp, "\tjne comp_label%d\n", comp_label);
            codeGenASTTraversal(cur_case_stmt->child_2);
            fprintf(fp, "\tjmp comp_label%d\n", fin_label);
            fprintf(fp, "comp_label%d:\n", comp_label);
            comp_label++;
            cur_case_stmt = cur_case_stmt->child_3;
        }
        fprintf(fp, "comp_label%d:\n", fin_label);
    }
}



void codeGenASTTraversal (Ast_Node* cur_ast_node) {
    if (!cur_ast_node) return;
    switch (cur_ast_node->type)
    {
    case 0:
        // for teminals
        break;
    case 1:
        // program
        fprintf (fp, "\tglobal main\n");
        fprintf (fp, "\textern printf, scanf, puts\n");
        fprintf (fp, "\tsection .text\n");
        codeGenASTTraversal(cur_ast_node->child_2);
        codeGenASTTraversal(cur_ast_node->child_3);
        codeGenASTTraversal(cur_ast_node->child_4);
        break;
    case 2:
        // module declarations
        break;
    case 3:
        // module declarations
        break;
    case 4:
        // other modules
        codeGenASTTraversal(cur_ast_node->child_1);
        codeGenASTTraversal(cur_ast_node->syn_next);
        break;
    case 5:
        // driver module
        fprintf (fp, "main:\n");
        fprintf (fp, "\tpush rax\n"); // stack alignment
        codeGenASTTraversal(cur_ast_node->child_1);
        fprintf (fp, "\tpop rax\n"); // stack alignment
        fprintf (fp, "\tret\n");
        break;
    case 6:
        // module // maybe include inputplist and ret
        fprintf (fp, "%s:\n", cur_ast_node->child_1->token_data->lexeme);
        fprintf (fp, "\tpush rax\n"); // stack alignment
        codeGenASTTraversal(cur_ast_node->child_4);
        fprintf (fp, "\tpop rax\n"); // stack alignment
        fprintf (fp, "\tret\n");
        break;
    case 7:
        // ret - outputPlist
        break;
    case 8:
        // inputPlist
        // setArrayBounds (cur_ast_node->child_1);
        codeGenASTTraversal (cur_ast_node->syn_next);
        break;
    case 9:
        // outputPlist
        break;
    case 10:
        // data type
        // this would only come from declare and not from inputPlist
        // if (cur_ast_node->child_2) {
        //     codeGenASTTraversal(cur_ast_node->child_1);
        //     // range arrays me only bound checking
        // }
        break;
    case 11:
        // range arrays
        break;
    case 12:
        // type
        break;
    case 13:
        // moduleDef
        codeGenASTTraversal(cur_ast_node->child_2);
        break;
    case 14:
        // statements
        codeGenASTTraversal(cur_ast_node->child_1);
        codeGenASTTraversal(cur_ast_node->syn_next);
        break;
    case 15:
        // statement
        break;
    case 16:
        // ioStatement - input
        getValue(cur_ast_node->child_1);
        break;
    case 17:
        // ioStatement - output
        printValue(cur_ast_node->child_1);
        break;
    case 18:
        // N_14
        break;
    case 19:
        // var_print
        break;
    case 20:
        // var_print
        break;
    case 21:
        // P1
        break;
    case 22:
        // assignmentStatement
        assignStatement(cur_ast_node);
        break;
    case 23:
        // lvalueIDstmt
        // calculate offset
        // assignIDtoVal(offset, current);
        break;
    case 24:
        // lvalueArrstmt
        // calculate offset
        // assignIDtoVal(offset, current);
        break;
    case 25:
        // index_arr
        break;
    case 26:
        // moduleReuseStmt
        // set the value of actual parameters to offsets of formal parameters of function
        // after the function has been processed
        // set the value of output list of function to offsets of optional list
        // if we pass array in input list, check bounds and transfer array data from callee to caller
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        break;
    case 27:
        // actual_para_list
        break;
    case 28:
        // singular_para_list
        break;
    case 29:
        // singular_para_list
        break;
    case 30:
        // N13
        break;
    case 31:
        // N11
        break;
    case 32:
        // optional
        // don't call id list -- NO USE
        break;
    case 33:
        // id list
        setArrayBounds(cur_ast_node->child_1);
        codeGenASTTraversal(cur_ast_node->syn_next);//////////////////////////////////////////////////////////////////////////////////////////////////
        break;
    case 34:
        // U
        // if cur node is plus then return value of ans
        // else store negative of answer in a temporary and return
        evaluateExpression(cur_ast_node);
        break;
    case 35:
        // 
        evaluateExpression(cur_ast_node);
        break;
    case 36:
        // for teminals
        break;
    case 37:
        // for teminals
        break;
    case 38:
        // for teminals
        break;
    case 39:
        // for teminals
        break;
    case 40:
        // for teminals
        break;
    case 41:
        // for teminals
        break;
    case 42:
        // for teminals
        break;
    case 43:
        // for teminals
        break;
    case 44:
        // for teminals
        break;
    case 45:
        // declare statement
        codeGenASTTraversal(cur_ast_node->child_2);
        break;
    case 46:
        // switch
        switchCaseStatements(cur_ast_node);
        break;
    case 47:
        // switch
        break;
    case 48:
        // iterativeStmt - for loop
        forStatement(cur_ast_node);
        break;
    case 49:
        // iterativeStmt - while loop
        
        break;
    case 50:
        // for loop
        
        break;
    case 51:
        // index - for loop
        
        break;
    case 52:
        // switch
        
        break;
    }
}

void codegen (Ast_Node* root) {
    fp = fopen("code.asm", "w+");
    initialiseStorage();
    codeGenASTTraversal(root);
    printDataSection();
    fclose(fp);
    printf("Chalgaya\n");
}