#include "../tree.h"

static void allignRank (int (*printFunc)(const char*, ...), size_t rank)
{
    assert (printFunc);
    for (size_t i = 0; i < rank; i++) printFunc ("\t");
}

#define allignDump(rank, str, ...)  allignRank (log_string, rank); log_string (str, __VA_ARGS__)

#define DATAPLACE(ord) \
    if (printfunc && order == ord) \
    { \
        allignRank (log_string, rank); \
        printfunc (node->data, node->size); \
    }

void TNdump_ (TreeNode* node, void (*printfunc)(const void*, size_t), size_t rank, DUMPORDER order)
{
    if (node == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return;
    }

    allignRank (log_string, rank);
    log_string ("{\n");

    allignDump (rank, "<blu>this ptr: %p <dft>\n", node);
    allignDump (rank, "data ptr:      %p\n",       node->data);
    allignDump (rank, "sizeof (data): %zu\n",      node->size);
    allignDump (rank, "parent:        %p\n",       node->parent);
    allignDump (rank, "left:          %p\n",       node->left);
    allignDump (rank, "right:         %p\n",       node->right);

    DATAPLACE (PRE)

    if (node->left)  TNdump_ (node->left,  printfunc, rank + 1, order);

    DATAPLACE (IN)

    if (node->right) TNdump_ (node->right, printfunc, rank + 1, order);

    DATAPLACE (POST)

    allignRank (log_string, rank);
    log_string ("}\n");
}
#undef DATAPLACE
#undef allignDump

void TRdump_ (const char* name, TreeRoot* root, void (*printfunc)(const void*, size_t), DUMPORDER order)
{
    if (root == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("verification error", "received NULL");
        return;
    }

    log_string
    (
        "<blu><b>dump of %s</b>(%p)<dft>\n"
        "size:  %zu\n"
        "depth: <blk>*not_implemented*<dft>\n"
        "root:  %p\n"
        "tree dump:\n",
        name,
        root,
        root->size,
        root->root
    );

    if (root->root) TNdump_ (root->root, printfunc, 0, order);
}