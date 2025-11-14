#include "hashTree.h"


int hashCMP (const void* a, const void* b)
{
    assert (a);
    assert (b);

    if      (((const HashTNode*)a)->hash > ((const HashTNode*)b)->hash) return  1;
    else if (((const HashTNode*)a)->hash < ((const HashTNode*)b)->hash) return -1;
    else                                                                return  0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void printHN (const void* data, size_t size)
{
    log_string ("%s : %lu\n", ((const HashTNode*)data)->node->data, ((const HashTNode*)data)->hash);
}
#pragma GCC diagnostic pop


static HashTNode* HTNinit (TreeNode* node)
{
    assertStrict (TNverify (node) == OK, "node failed verification");

    HashTNode* hashNode = (HashTNode*)calloc (1, sizeof (HashTNode));
    if (hashNode == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        return NULL;
    }

    hashNode->node = node;
    hashNode->hash = djb2Hash ((const char*)node->data, node->size);

    return hashNode;
}

static void HTNdel (HashTNode* hashNode)
{
    assertStrict (hashNode, "receivedNULL");
    
    free (hashNode);
}


HashTR* hashTR (TreeRoot* root)
{
    assertStrict (TRverify (root, NULL) == OK, "tree failed verification");

    TreeRoot* hashRoot = TRinit ();

    
    StackHandler stack = stackInit (root->size, sizeof (TreeNode*));
    for (TreeNode* top = root->root; top || stackLen (stack) > 0;)
    {
        if (stackLen (stack) > 0) stackPop (stack, &top);

        for (size_t i = 0; i < root->size && top; i++)
        {
            if (top->left == NULL && top->right == NULL)
            {
                HashTNode* hashNode = HTNinit (top);
                TNpush (hashRoot, hashNode, sizeof (HashTNode), hashCMP);
                HTNdel (hashNode);
            }

            if (top->right) stackPush (stack, &top->right);
            top = top->left;
        }
    }

    HashTR* hashTree = (HashTR*)calloc (1, sizeof (HashTR));
    if (hashTree == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        TRdel (hashRoot);
        return NULL;
    }

    hashTree->tree     = hashRoot;
    hashTree->original = root;

    return hashTree;
}

void HTRdel (HashTR* hashTree)
{
    if (hashTree == NULL)
    {
        log_war ("warning", "hashTree is NULL");
        return;
    }
    if (hashTree->tree) TRdel (hashTree->tree);
    free (hashTree);
}

TSTATUS HTRverify (const HashTR* hashTree)
{
    if (hashTree == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("verification error", "received NULL");
        return TERRNO;
    }
    if (TRverify (hashTree->original, NULL) != OK)
    {
        log_err ("verification failed", "original tree failed verification");
    }
    if (TRverify (hashTree->tree, hashCMP) != OK)
    {
        log_err ("verification failed", "hash tree failed verification");
    }

    return TERRNO;
}


HashTNode* HTNsearch (const HashTR* hashTree, hash_t targetHash)
{
    assertStrict (HTRverify (hashTree) == OK, "hashRoot failed verification");

    HashTNode hashNode = {
        .hash = targetHash,
        .node = NULL
    };

    TreeNode* node = TNsearch (hashTree->tree, &hashNode, hashCMP);
    if       (node == NULL)
    {
        log_err ("warning", "not founded");
        return NULL;
    }

    return (HashTNode*)(node->data);
}

TreeNode* HTNpush (HashTR* hashRoot, TreeNode* src)
{
    assertStrict (TRverify (hashRoot->tree, hashCMP)            == OK, "hashRoot failed verification");
    assertStrict (TNverify (src)                                == OK, "node failed verification");

    HashTNode* hashNode = HTNinit (src);
    if (hashNode == NULL)
    {
        TERRNO |= BADALLOC;
        log_err ("allocation error", "calloc returned NULL");
        return NULL;
    }

    TreeNode* res = TNpush (hashRoot->tree, hashNode, sizeof (HashTNode), hashCMP);

    HTNdel (hashNode);

    return res;
}