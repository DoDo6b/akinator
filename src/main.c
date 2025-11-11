#include "included/logger/logger.h"
#include "tree/tree.h"
#include "akinator/akinator.h"

static void printStr (const void* src, size_t size)
{
    log_string ("data:          %s\n", (const char*)src);
}
#define TNdumpStr(node)  TNdump_ (       node, printStr, 0, PRE)
#define TRdumpStr(root)  TRdump_ (#root, root, printStr,    PRE)

int main ()
{
    log_start ("stdout");

    TreeRoot* root = TRloadf ("save.txt");

    play (root);

    TRdumpStr (root);
    TRvdump   ("graph.dot", root);
    TRsavetof (root, "save.txt");

    TRdel (root);

    return 0;
}