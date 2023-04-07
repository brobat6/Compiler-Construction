/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "tree.h"

treeNode* createTreeNode(grammar_symbol * gs, struct token_info token)
{
    treeNode* new = (treeNode*)malloc(sizeof(treeNode));
    new->symbol=gs;
    new->token=token;
    new->firstchild=NULL;
    new->lastchild=NULL;
    new->next=NULL;
    new->prev=NULL;
    new->parent=NULL;
    new->grammar_rule=-1;
    return new;
}

void addChild(treeNode *parent, treeNode *child) {
  if (parent->firstchild == NULL) {
    parent->firstchild = child;
    parent->lastchild = child;
  }
  else{
    treeNode* temp=parent->firstchild;
    child->next=temp;
    temp->prev=child;
    parent->firstchild=child;
  }
  child->parent = parent;
  child->prev = NULL;
}

treeNode* nextLeaf (treeNode* cur_node) {
  if (cur_node == NULL) {
    return NULL;
  }
  if (cur_node->parent == NULL) {
    // printf("Error found in tree\n");
    return NULL;
  }
  if (cur_node->next == NULL) {
    return nextLeaf(cur_node->parent);
  }
  return cur_node->next;
}

void deallocateTree(treeNode* root){
    if(root==NULL)
    {
        return;
    }
    else if(root->firstchild==NULL)
    {
        free(root);
        return;
    }
    treeNode* curNode=root->firstchild;
    while(curNode->next!=NULL)
    {
        curNode=curNode->next;
        free(curNode->prev);
    }
    free(root->lastchild);
    free(root);
}