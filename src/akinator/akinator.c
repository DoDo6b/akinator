#include "akinator.h"


ASTATUS AERRNO = OK;

static ASTATUS addNewCh (TreeNode* parent);

ASTATUS play (TreeRoot* dbroot)
{
    if (dbroot == NULL)
    {
        AERRNO |= NULLRECIVED;
        log_err ("verification error", "received NULL");
        return AERRNO;
    }

    for (TreeNode* questionNode = dbroot->root, *parent = NULL; questionNode;)
    {
        printf ("%s?\n", (const char*)questionNode->data);

        char          userans = 0;
        scanf ("%c", &userans);
        
        while (userans != 'y' && userans != 'n')
        {
            printf ("allowed answer: y/n\n");
            scanf ("%c", &userans);
        }

        parent                 = questionNode;
        if      (userans == 'y') questionNode = questionNode->right;
        else                     questionNode = questionNode->left;

        if (questionNode == NULL)
        {
            if (userans == 'y') printf ("GGs\n");
            else
            {
                addNewCh (parent);
                if (AERRNO != OK) return AERRNO;
                dbroot->size++;
            }
            
            return AERRNO;
        }
    }

    if (dbroot->root == NULL)
    {
        dbroot->root = TNinit ("unknown", sizeof ("unknown"));
        return AERRNO;
    }

    log_err ("unknown error", "something went wrong");
    return AERRNO;
}


static ASTATUS addNewCh (TreeNode* parent)
{
    parent->left = TNinit (parent->data, parent->size);
    if (parent->left == NULL)
    {
        AERRNO |= BADALLOC;
        log_err ("allocation error", "something went wrong while allocating new node");
        return AERRNO;
    }
    parent->left->parent = parent;


    char buf[BUFSIZ] = {};
    printf ("Who is it?\n");
    scanf ("%s", buf);

    parent->right = TNinit (buf, strlen (buf) + 1);
    if (parent->right == NULL)
    {
        AERRNO |= BADALLOC;
        log_err ("allocation error", "something went wrong while allocating new node");
        return AERRNO;
    }
    parent->right->parent = parent;


    printf ("How is he different from %s? He ...\n", (const char*)parent->data);
    scanf ("%s", buf);
    size_t bufLen = strlen (buf) + 1;

    if (parent->size < bufLen)
    {
        parent->data = realloc (parent->data, bufLen);
        if (parent->data == NULL)
        {
            AERRNO |= BADALLOC;
            log_err ("allocation error", "realloc returned NULL");
            return AERRNO;
        }
    }
    memcpy (parent->data, buf, bufLen);
    parent->size = bufLen;

    return AERRNO;
}