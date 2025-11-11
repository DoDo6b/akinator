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

    bufWStr (bufW, RAWPRINTFSPEC, (CastTo_t)node->data);

    bufWStr (bufW, "{");
    if (node->left)  TNsavetof (node->left,  bufW);
    bufWStr (bufW, "}{");
    if (node->right) TNsavetof (node->right, bufW);
    bufWStr (bufW, "}");
}


void TRsavetof (const TreeRoot* root, const char* filename)
{
    Buffer* bufW = bufInit (BSIZ);
    FILE* stream = bufFOpen (bufW, filename, "w+");

    TNsavetof (root->root, bufW);

    bufFree (bufW);
    fclose (stream);

    log_string ("<grn>Tree saved to file: %s<dft>\n", filename);
}



static TreeNode* TNloadf (Buffer* bufR, size_t* counter)
{
    assertStrict (bufVerify (bufR, 0) == 0 && bufR->mode == BUFREAD, "buffer failed verification");

    printf ("c: %c, buffer: %s\n", bufpeekc (bufR), bufR->bufpos);

    if (bufpeekc (bufR) == '{') bufSeek (bufR, 1, SEEK_CUR);
    if (bufpeekc (bufR) == '}')
    {
        bufSeek (bufR, 1, SEEK_CUR);
        return NULL;
    }

    char* stop = strchr (bufR->bufpos, '{');
    size_t len = (size_t)(stop - bufR->bufpos) + 1;
    if (stop == NULL || len == 1) return NULL;

    char* buffer = (char*)calloc (len, sizeof (char));
    bufScanf (bufR, "%[^{]", buffer);
    buffer[len - 1] = '\0';

    TreeNode* node = TNinit (buffer, len);
    free (buffer);

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

    return root;
}