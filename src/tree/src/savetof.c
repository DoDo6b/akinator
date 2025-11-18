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

    bufFree (bufW);
    fclose (stream);

    log_string ("<grn>Tree saved to file: %s<dft>\n", filename);
}



static TreeNode* TNloadf (Buffer* bufR, size_t* counter)
{
    assertStrict (counter, "received NULL");
    assertStrict (bufVerify (bufR, 0) == BUFOK && bufR->mode == BUFREAD, "buffer failed verification");
    
    if (TERRNO & PARSING) return NULL;

    if (bufpeekc (bufR) == '{')
    {
        log_err ("parsing error", "label expected (%lu)", bufTell (bufR));
        TERRNO |= PARSING;
        return NULL;
    }
    if (bufpeekc (bufR) == '}') return NULL;

    char*      stop = strchr (bufR->bufpos, '{');
    long len = stop - bufR->bufpos;
    if (stop == NULL || len == 0)
    {
        log_err ("parsing error", "{ expected (%ld)", bufTell (bufR));
        TERRNO |= PARSING;
        return NULL;
    }

    if (strstr (bufR->bufpos, "не") || strstr (bufR->bufpos, "Не"))
    {
        log_war ("warning", "negative question");
    }

    TreeNode* node = TNinit (bufR->bufpos, (size_t)len);
    bufSeek (bufR, len + 1, SEEK_CUR);


    node->left  = TNloadf (bufR, counter);
    if (TERRNO & PARSING) return node;

    if (bufGetc (bufR) != '}')
    {
        log_err ("parsing error", "} expected (%ld)", bufTell (bufR));

        TERRNO |= PARSING;
        return node;
    }

    if (bufGetc (bufR) != '{')
    {
        log_err ("parsing error", "{ expected (%ld)", bufR->name, bufTell (bufR));

        TERRNO |= PARSING;
        return node;
    }
    node->right = TNloadf (bufR, counter);
    if (TERRNO & PARSING) return node;

    if (bufGetc (bufR) != '}')
    {
        log_err ("parsing error", "} expected (%ld)", bufTell (bufR));

        TERRNO |= PARSING;
        return node;
    }

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

    assertStrict (TNverify (node) == OK, "node failed verification");

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
    if (root->root) root->size++;

    fclose (bufR->stream);
    bufFree (bufR);

    if (TERRNO & PARSING)
    {
        TRdel (root);
        root = NULL;
        log_err ("parsing error", "tree wasnt loaded propperly, calling TRdel()");
    }

    assertStrict (TRverify (root, NULL) == OK, "tree failed verification");

    return root;
}