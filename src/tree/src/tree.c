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

        node->data = calloc (size, sizeof (char));
    if (node->data == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        free (node);
        return NULL;
    }
    node->size = size;
    memcpy (node->data, src, size);

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


TreeNode* TNpush (TreeRoot* root, const void* src, size_t size, int (*dataCmp)(const void*, const void*))
{
    assert (TRverify (root, dataCmp) == OK);
    if (src == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return NULL;
    }

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
            if (TERRNO)
            {
                log_err ("runtime error", "cant init next tree node");
            }

            root->size++;
            return *insPlace;
        }
    }

    log_err ("unknown error", "something went wrong");
    return NULL;
}

void TNpop (TreeRoot* root, const void* src, int (*dataCmp)(const void*, const void*))
{
    assert (TRverify (root, dataCmp) == OK);
    if (src == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return;
    }

    for (TreeNode* i = root->root, **parent = &root->root; i != NULL;)
    {
        int cmp = dataCmp (src, i->data);

        if      (cmp > 0) 
        {
            parent = &i->right;
            i      =  i->right;
        }
        else if (cmp < 0)
        {
            parent = &i->left;
            i      =  i->left;
        }
        else
        {
            free (i->data);
            i->size = 0;

            TreeNode*  replacement = NULL;

            if      (!i->right && !i->left)
            {
                *parent = NULL;
                free (i);
            }
            else if (i->right && i->left)
            {
                replacement = i->right;
                TreeNode** replParent  = &i->right;

                while (replacement->left) 
                {
                    replParent  = &replacement->left;
                    replacement =  replacement->left;
                }

                i->data = replacement->data;
                i->size = replacement->size;

                memset (replacement, 0, sizeof (TreeNode));
                free   (replacement);
                *replParent = NULL;
            }
            else
            {
                if      (i->left && !i->right) replacement = i->left;
                else if (i->right && !i->left) replacement = i->right;

                *parent = replacement;

                memset (i, 0, sizeof (TreeNode));
                free   (i);
            }

            
            root->size--;
            return;
        }
    }

    log_err ("warning", "element wasnt found");
}