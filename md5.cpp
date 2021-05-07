#include <iostream>
#include <string>
#include <cstring>

#pragma warning( disable : 4996 ) /* Disable "unsafe sprintf" warning */

/* Определение констант для преобразований */

#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* В коде все целочисленные переменные определены типом uint32_t для соответствия стандартам алгортима MD5 */

using std::uint32_t;

class MD5
{
private:

    const static int BLOCK_SIZE = 64;

    unsigned char buffer[BLOCK_SIZE];
    unsigned char digest[16]; // Result
    uint32_t state[4];
    uint32_t count[2];

    bool done = false;

    unsigned char padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    void initialize()
    {
        done = false;

        count[0] = 0;
        count[1] = 0;

        /* Constants */
        state[0] = 0x67452301;
        state[1] = 0xefcdab89;
        state[2] = 0x98badcfe;
        state[3] = 0x10325476;
    }

    /* Смещение X влево на C байт */
    inline uint32_t rotateLeft(uint32_t x, uint32_t c) {
        return (x << c) | (x >> (32 - c));
    }


    /* Функция для перекодирования unsigned char в int32_t. Предполагается, что length % 4 == 0 */
    static void decode(uint32_t out[], const unsigned char input[], uint32_t length)
    {
        for (size_t i = 0, j = 0; j < length; i++, j += 4)
            out[i] = ((uint32_t)input[j]) | (((uint32_t)input[j + 1]) << 8) | (((uint32_t)input[j + 2]) << 16) | (((uint32_t)input[j + 3]) << 24);
    }

    /* Функция для перекодирования int32_t в unsigned char. Предполагается, что length % 4 == 0 */
    static void encode(unsigned char out[], const uint32_t input[], uint32_t length)
    {
        for (uint32_t i = 0, j = 0; j < length; i++, j += 4)
        {
            out[j] = input[i] & 0xff;
            out[j + 1] = (input[i] >> 8) & 0xff;
            out[j + 2] = (input[i] >> 16) & 0xff;
            out[j + 3] = (input[i] >> 24) & 0xff;
        }
    }

    inline uint32_t F(uint32_t x, uint32_t y, uint32_t z)
    {
        return x & y | ~x & z;
    }
    inline uint32_t G(uint32_t x, uint32_t y, uint32_t z)
    {
        return x & z | y & ~z;
    }
    inline uint32_t H(uint32_t x, uint32_t y, uint32_t z)
    {
        return x ^ y ^ z;
    }
    inline uint32_t I(uint32_t x, uint32_t y, uint32_t z)
    {
        return y ^ (x | ~z);
    }

    inline void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
    }
    inline void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
    }
    inline void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
    }
    inline void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + I(b, c, d) + x + ac, s) + b;
    }

    void transform(const unsigned char block[BLOCK_SIZE])
    {
        /* A,B,C,D - аккумуляторы */
        uint32_t A = state[0];
        uint32_t B = state[1];
        uint32_t C = state[2];
        uint32_t D = state[3];

        uint32_t x[16];

        decode(x, block, BLOCK_SIZE);

        /* Round 1 */
        FF(A, B, C, D, x[0],  S11, 0xd76aa478); /* 1 */
        FF(D, A, B, C, x[1],  S12, 0xe8c7b756); /* 2 */
        FF(C, D, A, B, x[2],  S13, 0x242070db); /* 3 */
        FF(B, C, D, A, x[3],  S14, 0xc1bdceee); /* 4 */
        FF(A, B, C, D, x[4],  S11, 0xf57c0faf); /* 5 */
        FF(D, A, B, C, x[5],  S12, 0x4787c62a); /* 6 */
        FF(C, D, A, B, x[6],  S13, 0xa8304613); /* 7 */
        FF(B, C, D, A, x[7],  S14, 0xfd469501); /* 8 */
        FF(A, B, C, D, x[8],  S11, 0x698098d8); /* 9 */
        FF(D, A, B, C, x[9],  S12, 0x8b44f7af); /* 10 */
        FF(C, D, A, B, x[10], S13, 0xffff5bb1); /* 11 */
        FF(B, C, D, A, x[11], S14, 0x895cd7be); /* 12 */
        FF(A, B, C, D, x[12], S11, 0x6b901122); /* 13 */
        FF(D, A, B, C, x[13], S12, 0xfd987193); /* 14 */
        FF(C, D, A, B, x[14], S13, 0xa679438e); /* 15 */
        FF(B, C, D, A, x[15], S14, 0x49b40821); /* 16 */

        /* Round 2 */
        GG(A, B, C, D, x[1],  S21, 0xf61e2562); /* 17 */
        GG(D, A, B, C, x[6],  S22, 0xc040b340); /* 18 */
        GG(C, D, A, B, x[11], S23, 0x265e5a51); /* 19 */
        GG(B, C, D, A, x[0],  S24, 0xe9b6c7aa); /* 20 */
        GG(A, B, C, D, x[5],  S21, 0xd62f105d); /* 21 */
        GG(D, A, B, C, x[10], S22, 0x2441453); /* 22 */
        GG(C, D, A, B, x[15], S23, 0xd8a1e681); /* 23 */
        GG(B, C, D, A, x[4],  S24, 0xe7d3fbc8); /* 24 */
        GG(A, B, C, D, x[9],  S21, 0x21e1cde6); /* 25 */
        GG(D, A, B, C, x[14], S22, 0xc33707d6); /* 26 */
        GG(C, D, A, B, x[3],  S23, 0xf4d50d87); /* 27 */
        GG(B, C, D, A, x[8],  S24, 0x455a14ed); /* 28 */
        GG(A, B, C, D, x[13], S21, 0xa9e3e905); /* 29 */
        GG(D, A, B, C, x[2],  S22, 0xfcefa3f8); /* 30 */
        GG(C, D, A, B, x[7],  S23, 0x676f02d9); /* 31 */
        GG(B, C, D, A, x[12], S24, 0x8d2a4c8a); /* 32 */

        /* Round 3 */
        HH(A, B, C, D, x[5], S31, 0xfffa3942); /* 33 */
        HH(D, A, B, C, x[8], S32, 0x8771f681); /* 34 */
        HH(C, D, A, B, x[11],S33, 0x6d9d6122); /* 35 */
        HH(B, C, D, A, x[14],S34, 0xfde5380c); /* 36 */
        HH(A, B, C, D, x[1], S31, 0xa4beea44); /* 37 */
        HH(D, A, B, C, x[4], S32, 0x4bdecfa9); /* 38 */
        HH(C, D, A, B, x[7], S33, 0xf6bb4b60); /* 39 */
        HH(B, C, D, A, x[10],S34, 0xbebfbc70); /* 40 */
        HH(A, B, C, D, x[13],S31, 0x289b7ec6); /* 41 */
        HH(D, A, B, C, x[0], S32, 0xeaa127fa); /* 42 */
        HH(C, D, A, B, x[3], S33, 0xd4ef3085); /* 43 */
        HH(B, C, D, A, x[6], S34, 0x4881d05); /* 44 */
        HH(A, B, C, D, x[9], S31, 0xd9d4d039); /* 45 */
        HH(D, A, B, C, x[12],S32, 0xe6db99e5); /* 46 */
        HH(C, D, A, B, x[15],S33, 0x1fa27cf8); /* 47 */
        HH(B, C, D, A, x[2], S34, 0xc4ac5665); /* 48 */

        /* Round 4 */
        II(A, B, C, D, x[0], S41, 0xf4292244); /* 49 */
        II(D, A, B, C, x[7], S42, 0x432aff97); /* 50 */
        II(C, D, A, B, x[14],S43, 0xab9423a7); /* 51 */
        II(B, C, D, A, x[5], S44, 0xfc93a039); /* 52 */
        II(A, B, C, D, x[12],S41, 0x655b59c3); /* 53 */
        II(D, A, B, C, x[3], S42, 0x8f0ccc92); /* 54 */
        II(C, D, A, B, x[10],S43, 0xffeff47d); /* 55 */
        II(B, C, D, A, x[1], S44, 0x85845dd1); /* 56 */
        II(A, B, C, D, x[8], S41, 0x6fa87e4f); /* 57 */
        II(D, A, B, C, x[15],S42, 0xfe2ce6e0); /* 58 */
        II(C, D, A, B, x[6], S43, 0xa3014314); /* 59 */
        II(B, C, D, A, x[13],S44, 0x4e0811a1); /* 60 */
        II(A, B, C, D, x[4], S41, 0xf7537e82); /* 61 */
        II(D, A, B, C, x[11],S42, 0xbd3af235); /* 62 */
        II(C, D, A, B, x[2], S43, 0x2ad7d2bb); /* 63 */
        II(B, C, D, A, x[9], S44, 0xeb86d391); /* 64 */

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
        uint32_t index = count[0] / 8 % BLOCK_SIZE;

        /* Update number of bits */
        if ((count[0] += (length << 3)) < (length << 3))
            count[1]++;
        count[1] += (length >> 29);

        /* Number of bytes we need to fill in buffer */
        uint32_t firstPart = 64 - index;
        uint32_t i = 0;

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
            uint32_t index = count[0] / 8 % 64;
            uint32_t padLength = (index < 56) ? (56 - index) : (120 - index);
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
/* End of class implementation */

int main(int argc, char const* argv[])
{
    std::cout << "MD5 of 'lesik' is : " << md5("lesik") << std::endl;

    return 0;
}
