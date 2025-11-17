#ifndef AKINATOR_H
#define AKINATOR_H


#include "../tree/tree.h"
#include "src/hashTree/hashTree.h"
#include "src/ASTATUS.h"

typedef EC_t ASTATUS;

ASTATUS play (HashTR* dbroot);

void printChDescr (const HashTR* hashTree, const char* character, TreeNode* start, size_t startIt);
void cmpCh        (const HashTR* hashTree, const char* chA, const char* chB);


#endif