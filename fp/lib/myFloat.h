#ifndef __MYFLOAT_H__
#define __MYFLOAT_H__

#include "myFloat_types.h"

class myFloat {
private:
	MYFLOAT_UTYPE mBits;
	int mNbits;
	int mEs;

public:
	myFloat(MYFLOAT_UTYPE bits, int nbits, int es);
    myFloat(int nbits, int es);

    bool isZero() const;                  // check for 0
    bool isNar() const;                   // check for NaR
    bool isNeg() const;                   // check for negative

    int nbits() const;                    // size in bits
    int ss() const;                       // sign size in bits
    int es() const;                       // exponent size in bits
    int fs() const;                       // fraction size in bits

    myFloat zero() const;                   // 0
    myFloat one() const;                    // 1
    myFloat nar() const;                    // NaR

    myFloat neg() const;                    // -x
    myFloat rec() const;                    // 1 / x
    // myFloat sqrt() const;                   // sqrt(x)

    myFloat add(const myFloat& p) const;      // x + p
    myFloat sub(const myFloat& p) const;      // x - p
    myFloat mul(const myFloat& p) const;      // x * p
    myFloat div(const myFloat& p) const;      // x / p

    bool eq(const myFloat& p) const;        // x == p
    bool gt(const myFloat& p) const;        // x > p
    bool ge(const myFloat& p) const;        // x >= p
    bool lt(const myFloat& p) const;        // x < p
    bool le(const myFloat& p) const;        // x <= p

    void set(myFloat p);                    // x = p                    // x = n
    void set(double n);                   // x = n

    double getDouble() const;             // n = x

    // debug
    void setBits(MYFLOAT_UTYPE bits);
    MYFLOAT_UTYPE getBits();
    void print();
};

myFloat operator+(const myFloat& a, const myFloat& b);
myFloat operator-(const myFloat& a, const myFloat& b);
myFloat operator*(const myFloat& a, const myFloat& b);
myFloat operator/(const myFloat& a, const myFloat& b);

myFloat operator-(const myFloat& a);

bool operator<(const myFloat&a , const myFloat& b);
bool operator<=(const myFloat&a , const myFloat& b);
bool operator>(const myFloat&a , const myFloat& b);
bool operator>=(const myFloat&a , const myFloat& b);
bool operator==(const myFloat&a , const myFloat& b);
bool operator!=(const myFloat&a , const myFloat& b);


#endif