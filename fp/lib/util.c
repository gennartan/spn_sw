#include "util.h"

bool util_is_zero_fp(MYFLOAT_UTYPE p)
{
    return p == MYFLOAT_ZERO;
}

bool util_is_nar_fp(MYFLOAT_UTYPE p)
{
    return p == MYFLOAT_NAR;
}

bool util_is_neg_fp(MYFLOAT_UTYPE p)
{
    return (MYFLOAT_STYPE)p < 0 && !util_is_nar_fp(p);
}

int util_ss_fp()
{
    return 1;
}


int util_es_fp(MYFLOAT_UTYPE p, int nbits, int es)
{
    return es;
}

int util_fs_fp(MYFLOAT_UTYPE p, int nbits, int es)
{
    int ss = util_ss_fp();

    return nbits - ss - es, 0;
}

MYFLOAT_UTYPE util_neg_fp(MYFLOAT_UTYPE p, int nbits)
{
    // reverse all bits and add one
    return LMASK(-LMASK(p, nbits), nbits);
}
