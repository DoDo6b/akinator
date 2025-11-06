#ifndef TSTATUS_H
#define TSTATUS_H


enum
{
    OK              = 0,
    NULLRECIVED     = 1,
    BADALLOC        = 2,
    
    DUPLSYMB        = 4,
    UNKERR          = 8,

    DATABUFNINIT    = 16,
    UNSORTED        = 32,
    ROOTSTATCOR     = 64,
};


#endif