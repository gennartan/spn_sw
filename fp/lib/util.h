#ifndef __MYFLOAT_UTIL_H__
#define __MYFLOAT_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "myFloat_types.h"

#ifdef __GNUC__
#if MYFLOAT_WIDTH > 32
#error "CLZ doesn't support 64-bit"
#endif
#define CLZ(n) \
    ((n) == 0 ? 8 * sizeof(n) : __builtin_clz(n))
#endif

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
#define LSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) << (shift))

// shift count wraps around on x86: https://stackoverflow.com/q/3871650
#define RSHIFT(bits, shift) \
    ((shift) >= (int)(8 * sizeof(bits)) ? 0 : (bits) >> (shift))

#define POW2(n) \
    (LSHIFT(1, (n)))

#define FLOORDIV(a, b) \
    ((a) / (b) - ((a) % (b) < 0))

#define MIN(a, b) \
    ((a) < (b) ? (a) : (b))

#define MAX(a, b) \
    ((a) > (b) ? (a) : (b))

#define LMASK(bits, size) \
    ((bits) & LSHIFT(MYFLOAT_MASK, MYFLOAT_WIDTH - (size)))

#define HIDDEN_BIT(frac) \
    (MYFLOAT_MSB | RSHIFT((frac), 1))

bool util_is_zero_fp(MYFLOAT_UTYPE p);
bool util_is_nar_fp(MYFLOAT_UTYPE p);
bool util_is_neg_fp(MYFLOAT_UTYPE p);

int util_ss_fp();
int util_fs_fp(MYFLOAT_UTYPE p, int nbits, int es);
int util_es_fp(MYFLOAT_UTYPE p, int nbits, int es);

MYFLOAT_UTYPE util_neg_fp(MYFLOAT_UTYPE p, int nbits);

#ifdef __cplusplus
}
#endif

#endif
