#include "../tree.h"
#include "../../included/kassert/kassert.h"
#include "../../included/stack/stack.h"
#include "../../included/buffer/buffer.h"

#define BSIZ BUFSIZ

#define NODECOLOR     "#FFFFE0"

#define WCONSTSTR(str)  bufWrite (bufW, str, sizeof (str) - 1)
#define WSTR(str, ...) \
    snprintf (buf, sizeof (buf), str, __VA_ARGS__); \
    bufWrite (bufW, buf, strlen (buf))

void TRvdump (const char* name, const TreeRoot* root)
{
    assertStrict (name, "received NULL");
    assertStrict (root, "received NULL");

    Buffer* bufW = bufInit(BSIZ);
    if (!bufW)
    {
        log_err("allocation error", "cant allocate buffer");
        return;
    }
    
    FILE* stream = bufFOpen(bufW, name, "w");
    if (!stream)
    {
        log_err("fs error", "cant open file");
        bufFree(bufW);
        return;
    }

    WCONSTSTR
    (
        "digraph Tree {\n"
        "    graph [splines=ortho, outputorder=edgesfirst];\n"
        "    node [shape=plaintext, fontname=\"Arial\"];\n"
        "    edge [fontname=\"Arial\", fontsize=8];\n\n"
    );

    char buf[BSIZ] = {0};

    StackHandler stack = stackInit (root->size, sizeof (TreeNode*));

    for (TreeNode* top = root->root; top || stackLen (stack) > 0;)
    {
        if (stackLen (stack) > 0) stackPop (stack, &top);

        while (top)
        {
            WSTR
            (
                "    node%p [label=<\n"
                "        <table border=\"1\" cellborder=\"0\" cellspacing=\"0\" bgcolor=\"" NODECOLOR "\">\n"
                "           <tr><td port=\"f0\">this  = %p</td></tr>\n"
                "           <tr><td port=\"f1\">data  = %d</td></tr>\n"
                "           <tr><td port=\"f2\">left  = ",
                top,
                top,
                *(int*)top->data
            );

            if (top->left == NULL)  { WCONSTSTR ("NULL"); }
            else                    { WSTR ("%p", top->left); }

            WCONSTSTR ("</td></tr>\n           <tr><td port=\"f3\">right = ");

            if (top->right == NULL) { WCONSTSTR ("NULL"); }
            else                    { WSTR ("%p", top->right); }
            
            WCONSTSTR ("</td></tr>\n");
            WCONSTSTR ("        </table>>];\n");

            if (top->left)  { WSTR ("    node%p:w -> node%p:n [color=\"green\", penwidth=1.5];\n", top, top->left); }
            if (top->right) { WSTR ("    node%p:e -> node%p:n [color=\"green\", penwidth=1.5];\n", top, top->right); }
            WCONSTSTR ("\n");

            if (top->right) stackPush (stack, &top->right);
            top = top->left;
        }
    }

    WCONSTSTR ("}\n");

    bufFlush (bufW);
    bufFree  (bufW);
    fclose   (stream);
    
    log_string ("<grn>Graphviz dot file created: %s<dft>\n", name);
}

#undef WCONSTSTR
#undef WSTR