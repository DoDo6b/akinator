#ifndef AKINATOR_H
#define AKINATOR_H


#include "../tree/tree.h"
#include "src/hashTree/hashTree.h"
#include "src/ASTATUS.h"

typedef EC_t ASTATUS;

ASTATUS play (HashTR* dbroot);

void printChDescr (HashTR* hashTree, const char* character);


#endif