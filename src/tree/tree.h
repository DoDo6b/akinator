#ifndef TREE_H
#define TREE_H


#include <stdlib.h>
#include <string.h>

#include "../included/logger/logger.h"
#include "src/TSTATUS.h"

#define SECURE


typedef struct TreeNode_s
{
    void* data;
    size_t  size;

    struct TreeNode_s* left;
    struct TreeNode_s* right;

}TreeNode;
typedef struct
{
    TreeNode* root;

    size_t size;
    size_t depth;
}TreeRoot;

typedef uint32_t TSTATUS;
extern TSTATUS TERRNO;


TSTATUS TNverify (const TreeNode* node);
TSTATUS TRverify (const TreeRoot* root, int (*dataCmp)(const void*, const void*));

TreeNode* TNinit (const void* src, size_t size);
void      TNdel  (TreeNode* target);

TreeNode* TNpush (TreeRoot* root, const void* src, size_t size, int (*dataCmp)(const void*, const void*));
void      TNpop  (TreeRoot* root, const void* src,              int (*dataCmp)(const void*, const void*));


typedef enum
{
    PRE,
    POST,
    IN,
}DUMPORDER;
void      TNdump_ (                  TreeNode* node, void (*printfunc)(const void*, size_t), size_t rank, DUMPORDER order);
void      TRdump_ (const char* name, TreeRoot* root, void (*printfunc)(const void*, size_t),              DUMPORDER order);
#define   TNdump(node)  TNdump_ (       node, NULL, 0, PRE)
#define   TRdump(root)  TRdump_ (#root, root, NULL,    PRE)


#endif