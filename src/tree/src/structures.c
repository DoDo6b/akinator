#include "../tree.h"


TSTATUS TERRNO = OK;


TreeNode* TNinit (const void* src, size_t size)
{
    TreeNode* node = (TreeNode*)calloc (1, sizeof (TreeNode));
    if       (node == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        return NULL;
    }

        node->data = calloc (size + 1, sizeof (char));
    if (node->data == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        free (node);
        return NULL;
    }
    node->size = size;
    memcpy (node->data, src, size);
    *((char*)(node->data) + size) = 0;

    return node;
}

void TNdel (TreeNode* target)
{
    if (target)
    {
        if (target->data)  free  (target->data);
        if (target->left)  TNdel (target->left);
        if (target->right) TNdel (target->right);

        #ifdef SECURE
        memset (target, 0, sizeof (TreeNode));
        #endif
        
        free (target);
    }
    else
    {
        log_war ("warning", "target is NULL");
    }
}


TreeRoot* TRinit ()
{
    TreeRoot* root = (TreeRoot*)calloc (1, sizeof (TreeRoot));
    if       (root == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        return NULL;
    }
    return root;
}

void TRdel (TreeRoot* target)
{
    if (target)
    {
        if (target->root) TNdel (target->root);

        #ifdef SECURE
        memset (target, 0, sizeof (TreeRoot));
        #endif
        
        free (target);
    }
    else
    {
        log_war ("warning", "target is NULL");
    }
}