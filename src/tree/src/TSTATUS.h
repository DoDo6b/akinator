#ifndef TSTATUS_H
#define TSTATUS_H


enum
{
    OK              = 0,
    NULLRECIVED     = 1,
    BADALLOC        = 2,
    DEPMODULE       = 4,
    
    PARSING         = 8,
    DUPLSYMB        = 16,
    UNKERR          = 32,

    DATABUFNINIT    = 64,
    UNSORTED        = 128,
    ROOTSTATCOR     = 256,
};


#endif