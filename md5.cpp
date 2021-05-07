#include <iostream>
#include <string>
#include <cstdio>
#include <cmath>
#include <vector>
#include <string.h>

#pragma warning( disable : 4996 ) /* Disable "unsafe sprintf" warning */

/* В коде все целочисленные переменные определены типом uint32_t для соответствия стандартам алгортима MD5 */

using std::uint32_t;

class MD5
{
private:

    const static int BLOCK_SIZE = 64;

    unsigned char buffer[BLOCK_SIZE];
    unsigned char digest[16]; // Result
    int32_t state[4];
    int32_t count[2];

    bool done = false;

    /* K - таблица констант */
    uint32_t K[64] = { 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
                           0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                           0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
                           0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
                           0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
                           0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
                           0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
                           0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
                           0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
                           0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                           0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
                           0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
                           0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
                           0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                           0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
                           0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
    /* s - величины сдвигов для каждой операции */
    uint32_t s[64] = { 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                          5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
                          4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                          6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21 };

    unsigned char padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    /* a0, b0, c0, d0 - константы для MD5 */
    uint32_t a0 = 0x67452301;
    uint32_t b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe;
    uint32_t d0 = 0x10325476;

    void initialize()
    {
        done = false;

        count[0] = 0;
        count[1] = 0;

        state[0] = 0x67452301;
        state[1] = 0xefcdab89;
        state[2] = 0x98badcfe;
        state[3] = 0x10325476;
    }

    /* Смещение X влево на C байт */
    inline int32_t rotateLeft(uint32_t x, uint32_t c) {
        return (x << c) | (x >> (32 - c));
    }

    /* Функция для конвертации int переменной в массив байт */
    std::vector<unsigned char> intToBytes(uint32_t n)
    {
        std::vector<unsigned char> arrayOfByte(4);
        for (size_t i = 0; i < 4; ++i)
            arrayOfByte[3 - i] = (n >> (i * 8));
        return arrayOfByte;
    }

    /* Функция для перекодирования unsigned char в int32_t. Предполагается, что length % 4 == 0 */
    static void decode(int32_t out[], const unsigned char input[], int32_t length)
    {
        for (size_t i = 0, j = 0; j < length; i++, j += 4)
            out[i] = ((int32_t)input[j]) | (((int32_t)input[j + 1]) << 8) | (((int32_t)input[j + 2]) << 16) | (((int32_t)input[j + 3]) << 24);
    }

    /* Функция для перекодирования int32_t в unsigned char. Предполагается, что length % 4 == 0 */
    static void encode(unsigned char out[], const int32_t input[], int32_t length)
    {
        for (int32_t i = 0, j = 0; j < length; i++, j += 4)
        {
            out[j] = input[i] & 0xff;
            out[j + 1] = (input[i] >> 8) & 0xff;
            out[j + 2] = (input[i] >> 16) & 0xff;
            out[j + 3] = (input[i] >> 24) & 0xff;
        }
    }

    inline int32_t F(int32_t x, int32_t y, int32_t z)
    {
        return x & y | ~x & z;
    }
    inline int32_t G(int32_t x, int32_t y, int32_t z)
    {
        return x & z | x & ~z;
    }
    inline int32_t H(int32_t x, int32_t y, int32_t z)
    {
        return x ^ y ^ z;
    }
    inline int32_t I(int32_t x, int32_t y, int32_t z)
    {
        return y ^ (x | ~z);
    }

    inline void FF(int32_t& a, int32_t b, int32_t c, int32_t d, int32_t x, int32_t s, int32_t ac)
    {
        a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
    }
    inline void GG(int32_t& a, int32_t b, int32_t c, int32_t d, int32_t x, int32_t s, int32_t ac)
    {
        a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
    }
    inline void HH(int32_t& a, int32_t b, int32_t c, int32_t d, int32_t x, int32_t s, int32_t ac)
    {
        a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
    }
    inline void II(int32_t& a, int32_t b, int32_t c, int32_t d, int32_t x, int32_t s, int32_t ac)
    {
        a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
    }

    void transform(const unsigned char block[BLOCK_SIZE])
    {
        /* A,B,C,D - аккумуляторы */
        int32_t A = a0;
        int32_t B = b0;
        int32_t C = c0;
        int32_t D = d0;

        int32_t x[16];

        decode(x, block, BLOCK_SIZE);

        /* Round 1 */
        FF(A, B, C, D, x[0], s[11], 0xd76aa478); /* 1 */
        FF(D, A, B, C, x[1], s[12], 0xe8c7b756); /* 2 */
        FF(C, D, A, B, x[2], s[13], 0x242070db); /* 3 */
        FF(B, C, D, A, x[3], s[14], 0xc1bdceee); /* 4 */
        FF(A, B, C, D, x[4], s[11], 0xf57c0faf); /* 5 */
        FF(D, A, B, C, x[5], s[12], 0x4787c62a); /* 6 */
        FF(C, D, A, B, x[6], s[13], 0xa8304613); /* 7 */
        FF(B, C, D, A, x[7], s[14], 0xfd469501); /* 8 */
        FF(A, B, C, D, x[8], s[11], 0x698098d8); /* 9 */
        FF(D, A, B, C, x[9], s[12], 0x8b44f7af); /* 10 */
        FF(C, D, A, B, x[10], s[13], 0xffff5bb1); /* 11 */
        FF(B, C, D, A, x[11], s[14], 0x895cd7be); /* 12 */
        FF(A, B, C, D, x[12], s[11], 0x6b901122); /* 13 */
        FF(D, A, B, C, x[13], s[12], 0xfd987193); /* 14 */
        FF(C, D, A, B, x[14], s[13], 0xa679438e); /* 15 */
        FF(B, C, D, A, x[15], s[14], 0x49b40821); /* 16 */

        /* Round 2 */
        GG(A, B, C, D, x[1], s[21], 0xf61e2562); /* 17 */
        GG(D, A, B, C, x[6], s[22], 0xc040b340); /* 18 */
        GG(C, D, A, B, x[11], s[23], 0x265e5a51); /* 19 */
        GG(B, C, D, A, x[0], s[24], 0xe9b6c7aa); /* 20 */
        GG(A, B, C, D, x[5], s[21], 0xd62f105d); /* 21 */
        GG(D, A, B, C, x[10], s[22], 0x2441453); /* 22 */
        GG(C, D, A, B, x[15], s[23], 0xd8a1e681); /* 23 */
        GG(B, C, D, A, x[4], s[24], 0xe7d3fbc8); /* 24 */
        GG(A, B, C, D, x[9], s[21], 0x21e1cde6); /* 25 */
        GG(D, A, B, C, x[14], s[22], 0xc33707d6); /* 26 */
        GG(C, D, A, B, x[3], s[23], 0xf4d50d87); /* 27 */
        GG(B, C, D, A, x[8], s[24], 0x455a14ed); /* 28 */
        GG(A, B, C, D, x[13], s[21], 0xa9e3e905); /* 29 */
        GG(D, A, B, C, x[2], s[22], 0xfcefa3f8); /* 30 */
        GG(C, D, A, B, x[7], s[23], 0x676f02d9); /* 31 */
        GG(B, C, D, A, x[12], s[24], 0x8d2a4c8a); /* 32 */

        /* Round 3 */
        HH(A, B, C, D, x[5], s[31], 0xfffa3942); /* 33 */
        HH(D, A, B, C, x[8], s[32], 0x8771f681); /* 34 */
        HH(C, D, A, B, x[11], s[33], 0x6d9d6122); /* 35 */
        HH(B, C, D, A, x[14], s[34], 0xfde5380c); /* 36 */
        HH(A, B, C, D, x[1], s[31], 0xa4beea44); /* 37 */
        HH(D, A, B, C, x[4], s[32], 0x4bdecfa9); /* 38 */
        HH(C, D, A, B, x[7], s[33], 0xf6bb4b60); /* 39 */
        HH(B, C, D, A, x[10], s[34], 0xbebfbc70); /* 40 */
        HH(A, B, C, D, x[13], s[31], 0x289b7ec6); /* 41 */
        HH(D, A, B, C, x[0], s[32], 0xeaa127fa); /* 42 */
        HH(C, D, A, B, x[3], s[33], 0xd4ef3085); /* 43 */
        HH(B, C, D, A, x[6], s[34], 0x4881d05); /* 44 */
        HH(A, B, C, D, x[9], s[31], 0xd9d4d039); /* 45 */
        HH(D, A, B, C, x[12], s[32], 0xe6db99e5); /* 46 */
        HH(C, D, A, B, x[15], s[33], 0x1fa27cf8); /* 47 */
        HH(B, C, D, A, x[2], s[34], 0xc4ac5665); /* 48 */

        /* Round 4 */
        II(A, B, C, D, x[0], s[41], 0xf4292244); /* 49 */
        II(D, A, B, C, x[7], s[42], 0x432aff97); /* 50 */
        II(C, D, A, B, x[14], s[43], 0xab9423a7); /* 51 */
        II(B, C, D, A, x[5], s[44], 0xfc93a039); /* 52 */
        II(A, B, C, D, x[12], s[41], 0x655b59c3); /* 53 */
        II(D, A, B, C, x[3], s[42], 0x8f0ccc92); /* 54 */
        II(C, D, A, B, x[10], s[43], 0xffeff47d); /* 55 */
        II(B, C, D, A, x[1], s[44], 0x85845dd1); /* 56 */
        II(A, B, C, D, x[8], s[41], 0x6fa87e4f); /* 57 */
        II(D, A, B, C, x[15], s[42], 0xfe2ce6e0); /* 58 */
        II(C, D, A, B, x[6], s[43], 0xa3014314); /* 59 */
        II(B, C, D, A, x[13], s[44], 0x4e0811a1); /* 60 */
        II(A, B, C, D, x[4], s[41], 0xf7537e82); /* 61 */
        II(D, A, B, C, x[11], s[42], 0xbd3af235); /* 62 */
        II(C, D, A, B, x[2], s[43], 0x2ad7d2bb); /* 63 */
        II(B, C, D, A, x[9], s[44], 0xeb86d391); /* 64 */

        state[0] += A;
        state[1] += B;
        state[2] += C;
        state[3] += D;

        memset(x, 0, sizeof(x));
    }


public:

    MD5();
    MD5(const std::string& text)
    {
        initialize();
        update(text.c_str(), text.length());
        finalize();
    }

    /* Метод для обновления операции. Продолжает процесс хеширования, передавая следующий блок сообщения */
    void update(const unsigned char input[], uint32_t length)
    {
        /* Count number of bytes % 64 */
        int32_t index = count[0] / 8 % BLOCK_SIZE;

        /* Update number of bits */
        if ((count[0] += (length << 3)) < (length << 3))
            count[1]++;
        count[1] += (length >> 29);

        /* Number of bytes we need to fill in buffer */
        int32_t firstPart = 64 - index;
        int32_t i = 0;

        /* Transform as many times as possible */
        if (length >= firstPart)
        {
            /* Fill buffer, than transform */
            memcpy(&buffer[index], input, firstPart);
            transform(buffer);

            /* Transform chunks of blocksize(64 bytes) */
            for (i = firstPart; i + BLOCK_SIZE <= length; i += BLOCK_SIZE)
                transform(&input[i]);
            index = 0;
        }
        else
            i = 0;

        /* Remaining buffer input */
        memcpy(&buffer[index], &input[i], length - i);
    }
    void update(const char input[], uint32_t length)
    {
        update((const unsigned char*)input, length);
    }

    friend std::ostream& operator<<(std::ostream&, MD5 md5);

    MD5& finalize()
    {
        if (!done)
        {
            //Save number of bits
            unsigned char bits[8];
            encode(bits, count, 8);

            /* Pad out to 56 mod 64*/
            int32_t index = count[0] / 8 % 64;
            int32_t padLength = (index < 56) ? (56 - index) : (120 - index);
            update(padding, padLength);

            /* Append length (before padding) */
            update(bits, 8);

            /* Store state in digest */
            encode(digest, state, 16);

            memset(buffer, 0, sizeof(buffer));
            memset(count, 0, sizeof(count));

            done = true;
        }
        return *this;
    }

    std::string hexdigest() const
    {
        if (!done)
            return "";

        char buf[33];
        for (size_t i = 0; i < 16; i++)
            sprintf(buf + i * 2, "%02x", digest[i]);
        buf[32] = 0;

        return std::string(buf);
    }



};

std::ostream& operator<<(std::ostream& out, MD5 md5)
{
    return out << md5.hexdigest();
}

std::string md5(const std::string str)
{
    MD5 md5 = MD5(str);
    return md5.hexdigest();
}


int main(int argc, char const* argv[])
{
    //MD5 test;
    //test.printK();
    //std::cout << sizeof(uint32_t) << std::endl;

    std::cout << "MD5 of 'asdf' is : " << md5("asdf") << std::endl;

    return 0;
}
