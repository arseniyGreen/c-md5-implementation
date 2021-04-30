#include "md5.h"
#include <iostream>
#include <iomanip>
#include <windows.h>

using namespace std;

unsigned int rotateL(unsigned int x, int n)
{
    return ((x) << n) | ((x) >> (32 - n));

}

int main()
{
//The required hash value using existing md5 algorithm

MD5 md5 ;
cout << "empty str is:" << endl;
puts( md5.digestString( "" ) ) ;


//Initialize Vars
unsigned int a = 0x67452301;
unsigned int b = 0xefcdab89;
unsigned int c = 0x98badcfe;
unsigned int d = 0x10325476;

//Make a copy of above
unsigned int A = a;
unsigned int B = b;
unsigned int C = c;
unsigned int D = d;


//Assuming an empty string ""
//The message would be appended with 1 and then padded with 0's
//so m = 10000....until 512 bits
//is this true?
unsigned int m[16] = {0x80000000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

int r[64] =  {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22
    ,5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20
        ,4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23
        ,6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

unsigned int k[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
    ,0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501
        ,0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be
        ,0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
        ,0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa
        ,0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8
        ,0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed
        ,0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
        ,0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c
        ,0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70
        ,0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05
        ,0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
        ,0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039
        ,0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1
        ,0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
        ,0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };


// Do an ititial Round just for testing...
unsigned int f = 0;
cout << "Doing round " << dec << 1 << endl;
f = (b & c) | ((~b) & d);   //round 1
cout << "f is " << hex << f << endl;
unsigned int temp = d;
d = c;
c = b;
int i = 0;
int g = 0;
cout << "k is " << hex << k[0] << endl;
cout << "r is " << dec << r[0] << endl;
b = b^(rotateL( a ^ f ^ k[0] ^ m[0] ,r[0]));
a = temp;

cout << "a is " << hex << a << endl;
cout << "b is " << hex << b << endl;
cout << "c is " << hex << c << endl;
cout << "d is " << hex << d << endl;

for (int i=1;i<16;i++){
    cout << "Doing round " << dec << i+1 << endl;
    f = (b & c) | ((~b) & d);   //round 1
    g = i;
    cout << "f is " << hex << f << endl;
    //res = d & b | ~d & c; //round 2

    unsigned int temp = d;
    d = c;
    c = b;
    cout << "k is " << hex << k[i] << endl;
    cout << "r is " << dec << r[i] << endl;
    b = b^(rotateL( a ^ f ^ k[i] ^ m[g] ,r[i]));
    a = temp;

    cout << "a is " << hex << a << endl;
    cout << "b is " << hex << b << endl;
    cout << "c is " << hex << c << endl;
    cout << "d is " << hex << d << endl;
    system("PAUSE");
}

for (int i=16;i<32;i++){
    cout << "Doing round " << dec << i+1 << endl;
    f = d & b | ~d & c; //round 2
    cout << "f is " << hex << f << endl;

    g = ((5*i)+1)%16;

    unsigned int temp = d;
    d = c;
    c = b;
    cout << "k is " << hex << k[i] << endl;
    cout << "r is " << dec << r[i] << endl;
    b = b^(rotateL( a ^ f ^ k[i] ^ m[g] ,r[i]));
    a = temp;

    cout << "a is " << hex << a << endl;
    cout << "b is " << hex << b << endl;
    cout << "c is " << hex << c << endl;
    cout << "d is " << hex << d << endl;
    system("PAUSE");
}


for (int i=32;i<48;i++){
    cout << "Doing round " << dec << i+1 << endl;
    f = b ^ c ^ d;
    cout << "f is " << hex << f << endl;

    g = ((3*i)+5)%16;

    unsigned int temp = d;
    d = c;
    c = b;
    cout << "k is " << hex << k[i] << endl;
    cout << "r is " << dec << r[i] << endl;
    b = b^(rotateL( a ^ f ^ k[i] ^ m[g] ,r[i]));
    a = temp;

    cout << "a is " << hex << a << endl;
    cout << "b is " << hex << b << endl;
    cout << "c is " << hex << c << endl;
    cout << "d is " << hex << d << endl;
    system("PAUSE");
}

for (int i=48;i<64;i++){
    cout << "Doing round " << dec << i+1 << endl;
    f = c ^ (b | ~ d);
    cout << "f is " << hex << f << endl;

    g = ((7*i))%16;

    unsigned int temp = d;
    d = c;
    c = b;
    cout << "k is " << hex << k[i] << endl;
    cout << "r is " << dec << r[i] << endl;
    b = b^(rotateL( a ^ f ^ k[i] ^ m[g] ,r[i]));
    a = temp;

    cout << "a is " << hex << a << endl;
    cout << "b is " << hex << b << endl;
    cout << "c is " << hex << c << endl;
    cout << "d is " << hex << d << endl;
    system("PAUSE");
}

A = A ^ a;
B = B ^ b;
C = C ^ c;
D = D ^ d;

cout << "Finally: " << endl;
cout << "A is " << hex << A << endl;
cout << "B is " << hex << B << endl;
cout << "D is " << hex << C << endl;
cout << "D is " << hex << D << endl;

//The above would now be appended but they 
//do not equal to the required hash! 
//So clearly it is wrong??


return 0;