#include "included/logger/logger.h"
#include "tree/tree.h"


static int intCmp (const void* a, const void* b)
{
    if      (*(const int*)a > *(const int*)b) return 1;
    else if (*(const int*)a < *(const int*)b) return -1;
    else                                      return 0;
}

static void printInt (const void* src, size_t size)
{
    if (sizeof (int) != size)
    {
        log_err ("type error", "value type isnt int");
        return;
    }
    log_string ("data:          %d\n", *(const int*)src);
}
#define TNdumpInt(node)  TNdump_ (       node, printInt, 0, PRE)
#define TRdumpInt(root)  TRdump_ (#root, root, printInt,    PRE)

int main ()
{
    log_start ("stdout");

    TreeRoot root = {
        .root = NULL,
        .size = 0,
        .depth = 0,
    };

    int a = 10;
    TNpush (&root, &a, sizeof (a), intCmp);
    a = 5;
    TNpush (&root, &a, sizeof (a), intCmp);
    a = 3;
    TNpush (&root, &a, sizeof (a), intCmp);
    a = 7;
    TNpush (&root, &a, sizeof (a), intCmp);
    a = 20;
    TNpush (&root, &a, sizeof (a), intCmp);

    TRdumpInt (&root);
    log_string ("\n\n");

    a = 15;
    TNpush (&root, &a, sizeof (a), intCmp);

    TRdumpInt (&root);

    a = 20;
    TNpop (&root, &a, intCmp);
    TRdumpInt (&root);

    return 0;
}