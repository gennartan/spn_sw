#ifndef __MYFLOAT_PACK_H__
#define __MYFLOAT_PACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myFloat_types.h"

struct unpacked_t
{
    bool neg;
    MYFLOAT_STYPE exp;
    MYFLOAT_UTYPE frac;
};

MYFLOAT_UTYPE pack_myFloat(struct unpacked_t up, int nbits, int es);
double pack_double_fp(struct unpacked_t up);

struct unpacked_t unpack_myFloat(MYFLOAT_UTYPE p, int nbits, int es);
struct unpacked_t unpack_double_fp(double f);

#ifdef __cplusplus
}
#endif

#endif
