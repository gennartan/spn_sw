#include "pack.h"
#include "util.h"
#include <stdio.h>

void print_bits(MYFLOAT_UTYPE bits, int n_bits){
	for(int i=1;i<=n_bits;i++){
		printf("%d ", RSHIFT(bits, MYFLOAT_WIDTH - i) & 0x1);
	}
	printf("\n");
}

MYFLOAT_UTYPE pack_myFloat(struct unpacked_t up, int nbits, int es){
	MYFLOAT_UTYPE p;
	MYFLOAT_UTYPE fexpbits;

	// int maxexp = POW2(es) -1;
	// if(up.exp < -maxexp){
	// 	up.exp = -maxexp;
	// }else if(up.exp > maxexp){
	// 	up.exp = maxexp;
	// }
	int ss = util_ss_fp();
	int fs = util_fs_fp(p, nbits, es);

	int bias = POW2(es-1) - 1;
	int fexp = up.exp + bias;

	if(fexp > POW2(es)-2){
		// overflow
		fexpbits = LSHIFT(POW2(es)-2, fs);
	}else if(fexp < 1){
		// underflow
		fexpbits = 0;
	}else{
		fexpbits = LMASK(LSHIFT(fexp, MYFLOAT_WIDTH - es), es);
	}



	p = up.frac;
	p = fexpbits | RSHIFT(p, es);
	p = RSHIFT(p, ss);

	if(up.neg){
		return util_neg_fp(p, nbits);
	}else{
		return LMASK(p, nbits);
	}
}

double pack_double_fp(struct unpacked_t up){
	int fexp = up.exp + 1023;

	uint64_t fexpbits;
	uint64_t ffracbits;

	// not possible to overflow or underflow
	fexpbits = LSHIFT((uint64_t)(fexp & 0x7FF), 53);
	ffracbits = LSHIFT((uint64_t)up.frac, 64 - MYFLOAT_WIDTH);

	union {
		double f;
		uint64_t u;
	} un;

	un.u = ffracbits;
	un.u = fexpbits | RSHIFT(un.u, 11);
	un.u = LSHIFT((uint64_t)up.neg, 63) | RSHIFT(un.u, 1);

	if(LSHIFT(un.u,1)==0){
		un.u++;
	}

	return un.f;
}

struct unpacked_t unpack_myFloat(MYFLOAT_UTYPE p, int nbits, int es){
	struct unpacked_t up;
	int bias = POW2(es-1) - 1;

	int ss = util_ss_fp();

	MYFLOAT_UTYPE exp_mask = (LSHIFT(1, es)-1);

	up.neg = RSHIFT(p, MYFLOAT_WIDTH - ss);
	up.exp = (RSHIFT(p, MYFLOAT_WIDTH - es - ss) & exp_mask) - bias;
	up.frac = RSHIFT(LSHIFT(p, es+ss), 0);

	// print_bits(p, 32);
	// print_bits(up.exp, 32);
	// print_bits(up.frac, 32);


	return up;
}

struct unpacked_t unpack_double_fp(double f){
	struct unpacked_t up;
	int bias = 1023;

	union {
		double f;
		uint64_t u;
	} un;

	un.f = f;

	up.neg = RSHIFT(un.u, 63);
	up.exp = (RSHIFT(un.u, 52) & 0x7FF) - bias;
	up.frac = RSHIFT(LSHIFT(un.u, 12), 64 - MYFLOAT_WIDTH);

	// print_bits(up.neg, MYFLOAT_WIDTH);
	// print_bits(up.exp, MYFLOAT_WIDTH);
	// print_bits(up.frac, MYFLOAT_WIDTH);

	if(up.exp == -bias) {
		// ????
		printf("What is this line ?\n");
		up.exp -= CLZ(up.frac);
		up.frac = LSHIFT(up.frac, CLZ(up.frac) + 1);
	}

	return up;
}