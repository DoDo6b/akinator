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

    bufWStr (bufW, "{");
    bufWStr (bufW, RAWPRINTFSPEC, *(int*)node->data);

    if (node->left)  TNsavetof (node->left,  bufW);
    if (node->right) TNsavetof (node->right, bufW);

    bufWStr (bufW, "}");
}


void TRsavetof (const TreeRoot* root, const char* filename)
{
    Buffer* bufW = bufInit (BSIZ);
    FILE* stream = bufFOpen (bufW, filename, "w+");

    bufWStr   (bufW, "{");
    TNsavetof (root->root, bufW);
    bufWStr   (bufW, "}");

    bufFree (bufW);
    fclose (stream);

    log_string ("<grn>Tree saved to file: %s<dft>\n", filename);
}