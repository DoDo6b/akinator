#ifndef HASHTREE_H
#define HASHTREE_H


#include "../../../included/kassert/kassert.h"
#include "../../../included/stack/stack.h"
#include "../../../tree/tree.h"


typedef struct
{
    hash_t    hash;
    TreeNode* node;
}HashTNode;

int  hashCMP (const void* a, const void* b);
void printHN (const void* data, size_t size);


typedef struct
{
    TreeRoot* original;
    TreeRoot* tree;
}HashTR;

HashTR* hashTR (TreeRoot* root);
void    HTRdel (HashTR*   hashTree);

HashTNode* HTNsearch (const HashTR* hashTree, hash_t          targetHash);
TreeNode* HTNpush    (      HashTR* hashRoot, TreeNode*       src);

TSTATUS HTRverify    (const HashTR* hashTree);


#endif