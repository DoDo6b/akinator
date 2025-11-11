#include "akinator.h"
#include "locale.h"


ASTATUS AERRNO = OK;

static ASTATUS addNewCh (TreeNode* parent);

ASTATUS play (TreeRoot* dbroot)
{
    setlocale(LC_ALL, "");

    if (dbroot == NULL)
    {
        AERRNO |= NULLRECIVED;
        log_err ("verification error", "received NULL");
        return AERRNO;
    }

    for (TreeNode* questionNode = dbroot->root, *parent = NULL; questionNode;)
    {
        printf ("%s?\n", (const char*)questionNode->data);

        wchar_t       userans = 0;
        scanf ("%lc", &userans);
        
        while (userans != L'д' && userans != L'н')
        {
            printf ("разрешенные ответы: д/н\n");
            scanf ("%lc", &userans);
        }

        parent                  = questionNode;
        if      (userans == L'д') questionNode = questionNode->right;
        else                      questionNode = questionNode->left;

        if (questionNode == NULL)
        {
            if (userans == L'д') printf ("GGs\n");
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

    size_t bufSiz = BUFSIZ;
    char* buf = (char*)calloc (bufSiz, sizeof (char));
    if (buf == NULL)
    {
        AERRNO |= BADALLOC;
        log_err ("allocation error", "something went wrong while allocating buffer");
        return AERRNO;
    }

    int     c = 0;
    while ((c = getchar()) != '\n' && c != EOF) {}

    printf ("Кто это?\n");
    getline (&buf, &bufSiz, stdin);
    buf[strcspn(buf, "\n")] = '\0';

    parent->right = TNinit (buf, strlen (buf) + 1);
    if (parent->right == NULL)
    {
        AERRNO |= BADALLOC;
        log_err ("allocation error", "something went wrong while allocating new node");
        return AERRNO;
    }
    parent->right->parent = parent;


    printf ("Чем оно/он/она отличается от %s? Продолжите: Он/оно/она ...\n", (const char*)parent->data);
    getline (&buf, &bufSiz, stdin);
    buf[strcspn(buf, "\n")] = '\0';

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

    free (buf);

    return AERRNO;
}