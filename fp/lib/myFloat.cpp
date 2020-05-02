#include "myFloat.h"
#include "util.h"
#include "pack.h"
#include "op2.h"

#include <cstdio>
#include <cmath>


using namespace std;

myFloat::myFloat(MYFLOAT_UTYPE bits, int nbits, int es) :
	mBits(bits),
	mNbits(nbits),
	mEs(es)
{}

myFloat::myFloat(int nbits, int es) :
	myFloat(MYFLOAT_ZERO, nbits, es)
{}

bool myFloat::isZero() const{
	return util_is_zero_fp(mBits);
}

bool myFloat::isNar() const{
	return util_is_nar_fp(mBits);
}

bool myFloat::isNeg() const{
	return util_is_neg_fp(mBits);
}

int myFloat::nbits() const{
	return mNbits;
}

int myFloat::ss() const{
	return util_ss_fp();
}

int myFloat::es() const{
	return util_es_fp(mBits, mNbits, mEs);
}

int myFloat::fs() const{
	return util_fs_fp(mBits, mNbits, mEs);
}

myFloat myFloat::zero() const{
	return myFloat(MYFLOAT_ZERO, mNbits, mEs);
}

myFloat myFloat::one() const{
    MYFLOAT_UTYPE fexpbits;
    MYFLOAT_UTYPE ffracbits;
    MYFLOAT_UTYPE p;

    int es = util_es_fp(p, mNbits, mEs);
    int ss = util_ss_fp();

    ffracbits = 0;
    fexpbits = LSHIFT(LSHIFT(1, es-1)-1, mNbits - es);

    p = ffracbits;
    p = fexpbits | RSHIFT(p, es);
    p = RSHIFT(p, ss);
	return myFloat(p, mNbits, mEs);
}

myFloat myFloat::nar() const{
	return myFloat(MYFLOAT_NAR, mNbits, mEs);
}

myFloat myFloat::neg() const{
	if(isNar()){
		return nar();
	}
	return myFloat(util_neg_fp(mBits, mNbits), mNbits, mEs);
}

myFloat myFloat::rec() const{
	if (isNar() || isZero()) {
		return nar();
	}
	return one().div(*this);
}

myFloat myFloat::add(const myFloat& p) const{
	if(isNar() || p.isNar()){
		return nar();
	}else if(isZero()){
		return p;
	}else if(p.isZero()){
		return *this;
	}else if(neg().eq(p)) {
		return zero();
	}

	unpacked_t aup = unpack_myFloat(mBits, mNbits, mEs);
    unpacked_t bup = unpack_myFloat(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_add(aup, bup);

    return myFloat(pack_myFloat(up, mNbits, mEs), mNbits, mEs);
}



myFloat myFloat::sub(const myFloat& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero()) {
        return p.neg();
    } else if (p.isZero()) {
        return *this;
    } else if (eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_myFloat(mBits, mNbits, mEs);
    unpacked_t bup = unpack_myFloat(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_sub(aup, bup);

    return myFloat(pack_myFloat(up, mNbits, mEs), mNbits, mEs);
}

myFloat myFloat::mul(const myFloat& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero() || p.isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_myFloat(mBits, mNbits, mEs);
    unpacked_t bup = unpack_myFloat(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_mul(aup, bup);

    return myFloat(pack_myFloat(up, mNbits, mEs), mNbits, mEs);
}

myFloat myFloat::div(const myFloat& p) const
{
    if (isNar() || p.isNar() || p.isZero()) {
        return nar();
    } else if (isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_myFloat(mBits, mNbits, mEs);
    unpacked_t bup = unpack_myFloat(p.mBits, p.mNbits, p.mEs);
    unpacked_t up = op2_div(aup, bup);

    return myFloat(pack_myFloat(up, mNbits, mEs), mNbits, mEs);
}

bool myFloat::eq(const myFloat& p) const
{
    return mBits == p.mBits;
}

bool myFloat::gt(const myFloat& p) const
{
    return (MYFLOAT_STYPE)mBits > (MYFLOAT_STYPE)p.mBits;
}

bool myFloat::ge(const myFloat& p) const
{
    return gt(p) || eq(p);
}

bool myFloat::lt(const myFloat& p) const
{
    return !gt(p) && !eq(p);
}

bool myFloat::le(const myFloat& p) const
{
    return !gt(p);
}

void myFloat::set(myFloat p)
{
    mBits = pack_myFloat(unpack_myFloat(p.mBits, p.mNbits, p.mEs), mNbits, mEs);
}

void myFloat::set(double n)
{
    switch (fpclassify(n)) {
    case FP_INFINITE:
    case FP_NAN:
        mBits = MYFLOAT_NAR;
        break;
    case FP_ZERO:
        mBits = MYFLOAT_ZERO;
        break;
    default:
        mBits = pack_myFloat(unpack_double_fp(n), mNbits, mEs);
        break;
    }
}

double myFloat::getDouble() const
{
    if (isZero()) {
        return 0.0;
    } else if (isNar()) {
        return 0.0 / 0.0;
    }

    return pack_double_fp(unpack_myFloat(mBits, mNbits, mEs));
}

void myFloat::setBits(MYFLOAT_UTYPE bits){
	mBits = LSHIFT(bits, MYFLOAT_WIDTH - mNbits);
}

MYFLOAT_UTYPE myFloat::getBits(){
    return RSHIFT(mBits, MYFLOAT_WIDTH - mNbits);
}

void myFloat::print()
{
    myFloat p = (isNeg() ? neg() : *this);

    printf("{%d, %d} ", mNbits, mEs);

    if (isNar()) {
        printf("NaR\n");
        return;
    }

    for (int i = MYFLOAT_WIDTH - 1; i >= MYFLOAT_WIDTH - mNbits; i--) {
        printf("%d", RSHIFT(mBits, i) & 1);
    }

    printf(" -> ");
    printf(isNeg() ? "-" : "+");

    for (int i = MYFLOAT_WIDTH - ss() - 1; i >= MYFLOAT_WIDTH - mNbits; i--) {
        printf("%d", RSHIFT(p.mBits, i) & 1);

        if (i != MYFLOAT_WIDTH - mNbits &&
            ((i == MYFLOAT_WIDTH - ss()) ||
             (i == MYFLOAT_WIDTH - ss() - mEs))) {
            printf(" ");
        }
    }

    printf(" = %lg\n", getDouble());
}


myFloat operator+(const myFloat& a, const myFloat& b){
    return a.add(b);
}

myFloat operator-(const myFloat& a, const myFloat& b){
    return a.sub(b);
}

myFloat operator*(const myFloat& a, const myFloat& b){
    return a.mul(b);
}

myFloat operator/(const myFloat& a, const myFloat& b){
    return a.div(b);
}

myFloat operator-(const myFloat& a){
    return a.neg();
}

bool operator<(const myFloat&a , const myFloat& b){
    return a.lt(b);
}

bool operator<=(const myFloat&a , const myFloat& b){
    return a.le(b);
}

bool operator>(const myFloat&a , const myFloat& b){
    return a.gt(b);
}

bool operator>=(const myFloat&a , const myFloat& b){
    return a.ge(b);
}

bool operator==(const myFloat&a , const myFloat& b){
    return a.eq(b);
}

bool operator!=(const myFloat&a , const myFloat& b){
    return !a.eq(b);
}
