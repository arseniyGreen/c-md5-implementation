#include <iostream>
#include <cstring>
#include <string>

typedef struct
{
    unsigned long int state[4];         /* state (ABCD) */
    unsigned long int count[2];         /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];           /* input buffer */
} MD5_CTX;

class MD5
{
private:
    void Transform(unsigned long int state[4], unsigned char block[64]);
    void Encode(unsigned char*, unsigned long int*, unsigned int);
    
public:
    void MD5Initialize(MD5_CTX*);
    void MD5Update(MD5_CTX*, unsigned char*, unsigned int);

MD5(){};
};

static unsigned char PADDING[64] = /* 0x80 = 128 */
{
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F,G,H,I - MD5 basic functions */
#define FunF(x, y, z) (((x) & (y)) | ((~x) & (z))) //First round (X and Y) or (notX and Z)
#define FunG(x, y, z) (((x) & (z)) | ((y) & (~z))) //Second round (X and Z) or (notZ and Y)
#define FunH(x, y, z) ((x) ^ (y) ^ (z)) //Third round X xor Y xor Z
#define FunI(x, y, z) ((y) ^ ((x) | (~z))) //Forth round Y xor (notZ or X)

/* rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += I ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }

int main()
{



return 0;
}