#include "op2.h"
#include "util.h"

static struct unpacked_t add(struct unpacked_t a, struct unpacked_t b, bool neg){
	struct unpacked_t r;

	MYFLOAT_LUTYPE afrac = HIDDEN_BIT(a.frac);
	MYFLOAT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
	MYFLOAT_LUTYPE frac;

	if(a.exp > b.exp){
		r.exp = a.exp;
		bfrac = RSHIFT(bfrac, a.exp - b.exp);
	}else{
		r.exp = b.exp;
		afrac = RSHIFT(afrac, b.exp - a.exp);
	}

	frac = afrac + bfrac;
	if(RSHIFT(frac, MYFLOAT_WIDTH) != 0){
		r.exp++;
		frac = RSHIFT(frac, 1);
	}

	r.neg = neg;
	r.frac = LSHIFT(frac, 1);

	return r;
}


static struct unpacked_t sub(struct unpacked_t a, struct unpacked_t b, bool neg){
    struct unpacked_t r;

    MYFLOAT_UTYPE afrac = HIDDEN_BIT(a.frac);
    MYFLOAT_UTYPE bfrac = HIDDEN_BIT(b.frac);
    MYFLOAT_UTYPE frac;

    if (a.exp > b.exp || (a.exp == b.exp && a.frac > b.frac)) {
        r.exp = a.exp;
        bfrac = RSHIFT(bfrac, a.exp - b.exp);
        frac = afrac - bfrac;
    } else {
        neg = !neg;
        r.exp = b.exp;
        afrac = RSHIFT(afrac, b.exp - a.exp);
        frac = bfrac - afrac;
    }

    r.neg = neg;
    r.exp -= CLZ(frac);
    r.frac = LSHIFT(frac, CLZ(frac) + 1);

    return r;
}

struct unpacked_t op2_mul(struct unpacked_t a, struct unpacked_t b){
    struct unpacked_t r;

    MYFLOAT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    MYFLOAT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    MYFLOAT_UTYPE frac = RSHIFT(afrac * bfrac, MYFLOAT_WIDTH);
    MYFLOAT_STYPE exp = a.exp + b.exp + 1;

    if ((frac & MYFLOAT_MSB) == 0) {
        exp--;
        frac = LSHIFT(frac, 1);
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = LSHIFT(frac, 1);

    return r;
}

struct unpacked_t op2_div(struct unpacked_t a, struct unpacked_t b){
    struct unpacked_t r;

    MYFLOAT_LUTYPE afrac = HIDDEN_BIT(a.frac);
    MYFLOAT_LUTYPE bfrac = HIDDEN_BIT(b.frac);
    MYFLOAT_STYPE exp = a.exp - b.exp;

    if (afrac < bfrac) {
        exp--;
        bfrac = RSHIFT(bfrac, 1);
    }

    r.neg = a.neg ^ b.neg;
    r.exp = exp;
    r.frac = LSHIFT(afrac, MYFLOAT_WIDTH) / bfrac;

    return r;
}

struct unpacked_t op2_add(struct unpacked_t a, struct unpacked_t b){
    if (a.neg == b.neg) {
        return add(a, b, a.neg);
    } else {
        return sub(a, b, a.neg);
    }
}

struct unpacked_t op2_sub(struct unpacked_t a, struct unpacked_t b){
    if (a.neg == b.neg) {
        return sub(a, b, a.neg);
    } else {
        return add(a, b, a.neg);
    }
}