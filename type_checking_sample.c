#include "ast.h"

int TypeCheckingDFS(Ast_Node* root) {


    if(root->type == 35 && root->child_2 != NULL) {
        // Means we are at N7 now.
        int right_type = TypeCheckingDFS(root->syn_next);
        int left_type = TypeCheckingDFS(root->inh_1);
        if(right_type != left_type) {
            // THROW AN ERROR.
        }
        return left_type;
    }
}