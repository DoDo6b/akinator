#include "../tree.h"
#include "../../included/stack/stack.h"

TSTATUS TNverify (const TreeNode* node)
{
    if (node == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("verification error", "received NULL");

        return TERRNO;
    }
    if (node->data == NULL)
    {
        TERRNO |= DATABUFNINIT;
        log_err ("verification error", "data buffer wasnt allocated");
    }

    return TERRNO;
}
TSTATUS TRverify (const TreeRoot* root, int (*dataCmp)(const void*, const void*))
{
    size_t realSiz   = 0;

    StackHandler stack = stackInit (root->size + 1, sizeof (TreeNode*));

    for (TreeNode* top = root->root; top || stackLen (stack) > 0;)
    {
        if (stackLen (stack) > 0) stackPop (stack, &top);

        for (size_t i = 0; i < root->size + 1 && top; i++)
        {
            TERRNO |= TNverify (top);
            realSiz++;
            if (top->left && dataCmp (top->left->data, top->data) >= 0)
            {
                TERRNO |= UNSORTED;
                log_err ("verification error", "tree is unsorted (lchild) %zu", realSiz);
            }
            if (top->right && dataCmp (top->right->data, top->data) <= 0)
            {
                TERRNO |= UNSORTED;
                log_err ("verification error", "tree is unsorted (rchild)");
            }

            if (top->right) stackPush (stack, &top->right);
            top = top->left;
        }
    }

    if (root->size != realSiz)
    {
        TERRNO |= ROOTSTATCOR;
        log_err ("verification error", "tree sizes differ");
    }
    return TERRNO;
}