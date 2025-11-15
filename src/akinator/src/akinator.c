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



static TreeNode* searchHNtoTN (const HashTR* hashTree, const char* character)
{
    assertStrict (TRverify (hashTree->tree, hashCMP)  == OK, "hash tree failed verification");

    HashTNode* hashNode = HTNsearch (hashTree, djb2Hash (character, strlen (character)));
    if (hashNode == NULL)
    {
        printf ("Object not founded\n");
        TRdump_ ("hash tree", hashTree->tree, printHN, PRE);
        return NULL;
    }

    if (hashNode->node == NULL)
    {
        AERRNO |= ANOTFOUND;
        log_err ("error", "original node is NULL");
        return NULL;
    }

    return hashNode->node;
}


static uint64_t bitPath (const TreeNode* start)
{
    assertStrict (TNverify (start) == OK, "node failed verification");
    uint64_t bitVec64 = 0;

    const TreeNode* node = start;
    unsigned char      i = 0;
    for (; node->parent != NULL; node = node->parent, i++) if (node->parent->right == node) bitVec64 |= 1ULL << i;
    if (node->parent == NULL) bitVec64 |= 1ULL << i;

    bitVec64 <<= __builtin_clzll (bitVec64);
    return __builtin_bitreverse64 (bitVec64);
}

void printChDescr (const HashTR* hashTree, const char* character)
{
    assertStrict (TRverify (hashTree->tree, hashCMP)  == OK, "hash tree failed verification");
    assertStrict (TRverify (hashTree->original, NULL) == OK, "original tree failed verification");

    const TreeNode* node = searchHNtoTN (hashTree, character);
    if (node == NULL)
    {
        printf ("Not found\n");
        return;
    }

    uint64_t bitpath = bitPath (node);
    
    TreeNode* inode = hashTree->original->root;
    for (size_t i = 1; inode && (inode->left || inode->right); i++)
    {
        printf ("%s?\n", (const char*)inode->data);
        if ((bitpath >> i) & 1ULL)
        {
            printf ("| Да\nv\n");

            inode = inode->right;
        }
        else
        {
            printf ("| Нет\nv\n");
            
            inode = inode->left;
        }
    }
    printf ("%s\n", (const char*)node->data);
}

void cmpCh (const HashTR* hashTree, const char* chA, const char* chB)
{
    assertStrict (chA, "received NULL");
    assertStrict (chB, "received NULL");

    assertStrict (TRverify (hashTree->tree, hashCMP)  == OK, "hash tree failed verification");
    assertStrict (TRverify (hashTree->original, NULL) == OK, "original tree failed verification");

    const TreeNode* nodeA = searchHNtoTN (hashTree, chA);
    if (nodeA == NULL)
    {
        printf ("Not found character A\n");
        return;
    }

    const TreeNode* nodeB = searchHNtoTN (hashTree, chB);
    if (nodeB == NULL)
    {
        printf ("Not found character B\n");
        return;
    }
    uint64_t          bitpathA             = bitPath (nodeA);
    uint64_t                      bitpathB = bitPath (nodeB);
    uint64_t difpath = bitpathA ^ bitpathB;

    TreeNode* inode = hashTree->original->root;
    for (size_t i = 1; inode && (inode->left || inode->right); i++)
    {   
        if ((difpath >> i) & 1ULL)
        {
            printf ("Развилка здесь: \"%s?\"\n", (const char*)inode->data);
            return;
        }

        inode = (bitpathA >> i) & 1ULL ? inode->right : inode->left;
    }

    AERRNO |= AUNKNOWN;
    log_err ("unknown error", "something went wrong");
}