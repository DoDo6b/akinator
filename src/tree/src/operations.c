#include "../tree.h"


TreeNode* TNpush (TreeRoot* root, const void* src, size_t size, int (*dataCmp)(const void*, const void*))
{
    assert (TRverify (root, dataCmp) == OK);
    if (src == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return NULL;
    }
    assert (dataCmp);

    if (root->root == NULL)
    {
        root->root = TNinit (src, size);
            if (TERRNO)
            {
                log_err ("runtime error", "cant init next tree node");
            }

            root->size++;
            return root->root;
    }

    for (TreeNode* i = root->root, **insPlace = NULL; i != NULL;)
    {
        int cmp = dataCmp (src, i->data);

        TreeNode* parent = i;
        if      (cmp > 0)
        {
            insPlace = &i->right;
            i        =  i->right;
        }
        else if (cmp < 0)
        {
            insPlace = &i->left;
            i        =  i->left;
        }
        else
        {
            TERRNO |= DUPLSYMB;
            log_err ("warning", "duplicate symbol");
            return i;
        }

        if (i == NULL)
        {
            *insPlace = TNinit (src, size);
            (*insPlace)->parent = parent;
            if (TERRNO)
            {
                log_err ("runtime error", "cant init next tree node");
            }

            root->size++;

            assert (TRverify (root, dataCmp) == OK);

            return *insPlace;
        }
    }

    log_err ("unknown error", "something went wrong");
    return NULL;
}

TreeNode* TNsearch (const TreeRoot* root, void* src, int (*dataCmp)(const void*, const void*))
{
    assert (TRverify (root, dataCmp) == OK);
    if (src == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return NULL;
    }
    assert (dataCmp);

    for (TreeNode* i = root->root; i;)
    {
        int cmp = dataCmp (src, i->data);

        if      (cmp > 0) i = i->right;
        else if (cmp < 0) i = i->left;
        else              return i;
    }

    return NULL;
}

void TNpop (TreeRoot* root, TreeNode* target, int (*dataCmp)(const void*, const void*))
{
    assert (TRverify (root, dataCmp) == OK);
    assert (TNverify (target)        == OK);
    assert (dataCmp);

    free (target->data);
    target->size = 0;

    TreeNode*  replacement = NULL;

    TreeNode** parentContainer = target->parent ? (target->parent->left == target ? &target->parent->left : &target->parent->right) : NULL;

    if      (!target->right && !target->left)
    {
        if (parentContainer) *parentContainer = NULL;
        else                 root->root       = NULL;

        free (target);
    }
    else if (target->right && target->left)
    {
        replacement = target->right;
        TreeNode** replParent  = &target->right;

        while (replacement->left) 
        {
            replParent  = &replacement->left;
            replacement =  replacement->left;
        }

        target->data = replacement->data;
        target->size = replacement->size;

        memset (replacement, 0, sizeof (TreeNode));
        free   (replacement);
        *replParent = NULL;
    }
    else
    {
        if      (target->left && !target->right) replacement = target->left;
        else if (target->right && !target->left) replacement = target->right;

        if (parentContainer) *parentContainer = replacement;
        else                       root->root = replacement;

        memset (target, 0, sizeof (TreeNode));
        free   (target);
    }

    
    root->size--;

    assert (TRverify (root, dataCmp) == OK);

    return;

    log_err ("warning", "element wasnt found");
}