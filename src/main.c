#include "included/logger/logger.h"
#include "tree/tree.h"
#include "akinator/akinator.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
static void printStr (const void* src, size_t size)
{
    log_string ("data:          %s\n", (const char*)src);
}
#pragma GCC diagnostic pop

#define TNdumpStr(node)  TNdump_ (       node, printStr, 0, PRE)
#define TRdumpStr(root)  TRdump_ (#root, root, printStr,    PRE)

int main (int argc, char** argv)
{
    log_start ("stdout");

    TreeRoot* root = NULL;

    if (argc >= 3 && strcmp (argv[1], "-s") == 0) root = TRloadf (argv[2]);
    else
    {
        printf ("usage: %s -s *save filename* ...\n", argv[0]);
        return 1;
    }
    TRdumpStr (root);

    HashTR* hashTree = hashTR (root);

    bool DIRTY = 0;
    if (argc >= 4)
    {
        for (int arg = 3; arg < argc; arg++)
        {
            if (strcmp (argv[arg], "-p") == 0)
            {
                play (hashTree);
                
                DIRTY = true;
            }
            else if (strcmp (argv[arg], "-d") == 0) 
            {
                char chname[BUFSIZ] = {0};
                printf ("type name: ");
                gets (chname);
                printf ("\n");

                printChDescr (hashTree, chname);
            }
            else if (strcmp (argv[arg], "-c") == 0)
            {
                char chA[BUFSIZ] = {0};
                printf ("type first name: ");
                gets (chA);

                char chB[BUFSIZ] = {0};
                printf ("\ntype second name: ");
                gets (chB);

                printf ("\n");

                cmpCh (hashTree, chA, chB);
            }
            else
            {
                printf ("unknown flag\n");
                break;
            }
        }
    }

    HTRdel (hashTree);

    TRvdump   ("graph.dot", root);

    if (DIRTY) TRsavetof (root, argv[2]);

    TRdel (root);

    return 0;
}