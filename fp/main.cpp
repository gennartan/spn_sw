#include "myFloat.h"
#include <stdio.h>
#include <bitset>
#include <string>
#include <iostream>

using namespace std;

void floatToBinary(float f, string& str)
{
    union { float f; uint32_t i; } u;
    u.f = f;
    str.clear();

    for (int i = 0; i < 32; i++)
    {
        if (u.i % 2)  str.push_back('1');
        else str.push_back('0');
        u.i >>= 1;
    }

    // Reverse the string since now it's backwards
    string temp(str.rbegin(), str.rend());
    str = temp;
}

#define NBITS 10
#define ES 5

int main(int argc, char *argv[]){
	myFloat f1 = myFloat(NBITS,ES);
	myFloat f2 = myFloat(NBITS,ES);
	myFloat f3 = myFloat(NBITS,ES);
	myFloat f4 = myFloat(NBITS,ES);

	for (int i=0;i<5;i++){
		f1.set(i);
		cout << i << " : bits " << bitset<NBITS>(f1.getBits()) << endl;
	}

	for(int i=0;i<5;i++){
		f1.set(i);
		cout << i << " : value : " << f1.getDouble() << endl;
	}
	f2.set(1.25);
	f3 = f1 + f2;
	f4 = f1 * f2;

	cout << f1.getDouble() << " + " << f2.getDouble() << " == " << f3.getDouble() << endl;
	cout << f1.getDouble() << " * " << f2.getDouble() << " == " << f4.getDouble() << endl;

	f1.print();
	return 0;

	cout << "bits " << bitset<NBITS>(f2.getBits()) << endl;
	cout << "bits " << bitset<NBITS>(f3.getBits()) << endl;
	cout << "bits " << bitset<NBITS>(f4.getBits()) << endl;
    return 0;
}