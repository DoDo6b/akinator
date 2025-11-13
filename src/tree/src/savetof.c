#include "../../included/buffer/buffer.h"
#include "../tree.h"



static void TNsavetof (const TreeNode* node, Buffer* bufW)
{
    if (node == NULL)
    {
        TERRNO |= NULLRECIVED;
        log_err ("badarg error", "received NULL");
        return;
    }
    assertStrict (bufVerify (bufW, 0) == BUFOK, "buffer failed verification");

    bufWStr (bufW, RAWPRINTFSPEC, (CastTo_t)node->data);

    bufWStr (bufW, "{");
    if (node->left)  TNsavetof (node->left,  bufW);
    bufWStr (bufW, "}{");
    if (node->right) TNsavetof (node->right, bufW);
    bufWStr (bufW, "}");
}


void TRsavetof (const TreeRoot* root, const char* filename)
{
    assertStrict (TRverify (root, NULL) == OK, "root failed verification");
    assertStrict (filename, "received NULL");

    Buffer* bufW = bufInit (BSIZ);
    FILE* stream = bufFOpen (bufW, filename, "w+");

    TNsavetof (root->root, bufW);

    fclose (stream);
    bufFree (bufW);

    log_string ("<grn>Tree saved to file: %s<dft>\n", filename);
}



static TreeNode* TNloadf (Buffer* bufR, size_t* counter)
{
    assertStrict (counter, "received NULL");
    assertStrict (bufVerify (bufR, 0) == BUFOK && bufR->mode == BUFREAD, "buffer failed verification");
    

    if (bufpeekc (bufR) == '{') bufSeek (bufR, 1, SEEK_CUR);
    if (bufpeekc (bufR) == '}')
    {
        bufSeek (bufR, 1, SEEK_CUR);
        return NULL;
    }

    char*      stop = strchr (bufR->bufpos, '{');
    long len = stop - bufR->bufpos;
    if (stop == NULL || len == 0) return NULL;

    TreeNode* node = TNinit (bufR->bufpos, (size_t)len);
    bufSeek (bufR, len, SEEK_CUR);

    node->left  = TNloadf (bufR, counter);
    node->right = TNloadf (bufR, counter);

    if (node->left)
    {
        node->left->parent = node;
        *counter += 1;
    }
    if (node->right)
    {
        node->right->parent = node;
        *counter += 1;
    }

    if (bufpeekc (bufR) == '}') bufSeek (bufR, 1, SEEK_CUR);

    assertStrict (TNverify (node) == 0, "node failed verification");

    return node;
}

TreeRoot* TRloadf (const char* filename)
{
    assertStrict (filename, "received NULL");

    Buffer* bufR = bufInit (BSIZ);
    bufFOpen (bufR, filename, "r");
    bufRead  (bufR, 0);

    TreeRoot* root = TRinit ();

    root->root = TNloadf (bufR, &root->size);
    root->size++;

    fclose (bufR->stream);
    bufFree (bufR);

    assertStrict (TRverify (root, NULL), "tree failed verification");

    return root;
}