#include "locale.h"
#include "hashTree/hashTree.h"
#include "../akinator.h"


ASTATUS AERRNO = AOK;

static ASTATUS addNewCh (TreeNode* parent, HashTR* hashTree);

ASTATUS play (HashTR* hashTree)
{
    setlocale(LC_ALL, "");

    if (TRverify (hashTree->original, NULL))
    {
        log_err ("verification error", "data base failed verification");
        return AERRNO;
    }

    for (TreeNode* questionNode = hashTree->original->root, *parent = NULL; questionNode;)
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
                addNewCh (parent, hashTree);
                if (AERRNO != OK) return AERRNO;
            }

            return AERRNO;
        }
    }


    if (hashTree->original->root == NULL)
    {
        hashTree->original->root = TNinit ("unknown", sizeof ("unknown"));
        return AERRNO;
    }

    log_err ("unknown error", "something went wrong");
    return AERRNO;
}


static ASTATUS addNewCh (TreeNode* parent, HashTR* hashTree)
{
    parent->left = TNinit (parent->data, parent->size);
    if (parent->left == NULL)
    {
        AERRNO |= BADALLOC;
        log_err ("allocation error", "something went wrong while allocating new node");
        return AERRNO;
    }
    parent->left->parent = parent;
    hashTree->original->size++;
    HTNsearch (hashTree, djb2Hash ((const char*)parent->data, parent->size))->node = parent->left;

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
    hashTree->original->size++;
    HTNpush (hashTree, parent->right);


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


void printChDescr (HashTR* hashTree, const char* character)
{
    assertStrict (HTRverify (hashTree) == OK, "hash tree failed verification");

    HashTNode* hashNode = HTNsearch (hashTree, djb2Hash (character, strlen (character)));
    if (hashNode == NULL)
    {
        printf ("Object not founded\n");
        TRdump_ ("hash tree", hashTree->tree, printHN, PRE);
        return;
    }

    const TreeNode* node = (const TreeNode*)(hashNode->node);
    if (node == NULL)
    {
        AERRNO |= NULLRECIVED;
        log_err ("error", "original node is NULL");
        return;
    }


    StackHandler stack = stackInit (hashTree->original->size * 3, sizeof (const char*));
    for (const TreeNode* i = node; i->parent != NULL; i = i->parent)
    {
        const char* buf = "v";
        stackPush (stack, &buf);

        buf = (i->parent == NULL || i->parent->right == i) ? "Да" : "Нет";
        stackPush (stack, &buf);

        buf = "|";
        stackPush (stack, &buf);
        
        stackPush (stack, &i->parent->data);
    }
    while (stackLen (stack) > 0)
    {
        const char* str = NULL;
        stackPop (stack, &str);
        printf ("%s\n", str);
    }

    stackFree (stack);

    printf ("%s\n", (const char*)node->data);
}