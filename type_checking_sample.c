#include "ast.h"

int TypeCheckingDFS(Ast_Node* root) {

    if(root->type == 35) {
        if(root->child_2 == NULL) {
            // we are at arithmeticOrBooleanExpression
            int type_of_expression = TypeCheckingDFS(root->syn_next);
            return type_of_expression;
        } else {
            int right_type = TypeCheckingDFS(root->syn_next);
            int left_type = TypeCheckingDFS(root->inh_1);
            if(right_type != left_type) {
                // THROW AN ERROR.
            }
            return left_type;
        }
    }
}