#ifndef TSTATUS_H
#define TSTATUS_H


enum
{
    OK              = 0,
    NULLRECIVED     = 1,
    BADALLOC        = 2,
    DEPMODULE       = 4,
    
    DUPLSYMB        = 8,
    UNKERR          = 16,

    DATABUFNINIT    = 32,
    UNSORTED        = 64,
    ROOTSTATCOR     = 128,
};


#endif