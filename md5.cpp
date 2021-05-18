#include <iostream>
#include <string>
#include <cstring>

#ifdef __unix__
    #define CLEAR system("clear")

#elif defined(_WIN32) || defined(WIN32)
    #define CLEAR system("cls")
#endif

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

     
    /*  Таблица констант для преобразований
        K[i] = floor(2^32 * abs(sin(i + 1)))  */
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
                       0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
                    };

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

    uint32_t F(uint32_t x, uint32_t y, uint32_t z)
    {
        return x & y | ~x & z;
    }
    uint32_t G(uint32_t x, uint32_t y, uint32_t z)
    {
        return x & z | y & ~z;
    }
    uint32_t H(uint32_t x, uint32_t y, uint32_t z)
    {
        return x ^ y ^ z;
    }
    uint32_t I(uint32_t x, uint32_t y, uint32_t z)
    {
        return y ^ (x | ~z);
    }

    void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + F(b, c, d) + x + ac, s) + b;
    }
    void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + G(b, c, d) + x + ac, s) + b;
    }
    void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
    {
        a = rotateLeft(a + H(b, c, d) + x + ac, s) + b;
    }
    void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac)
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
        FF(A, B, C, D, x[0 ], S11, K[0 ]); /* 1 */
        FF(D, A, B, C, x[1 ], S12, K[1 ]); /* 2 */
        FF(C, D, A, B, x[2 ], S13, K[2 ]); /* 3 */
        FF(B, C, D, A, x[3 ], S14, K[3 ]); /* 4 */
        FF(A, B, C, D, x[4 ], S11, K[4 ]); /* 5 */
        FF(D, A, B, C, x[5 ], S12, K[5 ]); /* 6 */
        FF(C, D, A, B, x[6 ], S13, K[6 ]); /* 7 */
        FF(B, C, D, A, x[7 ], S14, K[7 ]); /* 8 */
        FF(A, B, C, D, x[8 ], S11, K[8 ]); /* 9 */
        FF(D, A, B, C, x[9 ], S12, K[9 ]); /* 10 */
        FF(C, D, A, B, x[10], S13, K[10]); /* 11 */
        FF(B, C, D, A, x[11], S14, K[11]); /* 12 */
        FF(A, B, C, D, x[12], S11, K[12]); /* 13 */
        FF(D, A, B, C, x[13], S12, K[13]); /* 14 */
        FF(C, D, A, B, x[14], S13, K[14]); /* 15 */
        FF(B, C, D, A, x[15], S14, K[15]); /* 16 */

        /* Round 2 */
        GG(A, B, C, D, x[1 ], S21, K[16]); /* 17 */
        GG(D, A, B, C, x[6 ], S22, K[17]); /* 18 */
        GG(C, D, A, B, x[11], S23, K[18]); /* 19 */
        GG(B, C, D, A, x[0 ], S24, K[19]); /* 20 */
        GG(A, B, C, D, x[5 ], S21, K[20]); /* 21 */
        GG(D, A, B, C, x[10], S22, K[21]); /* 22 */
        GG(C, D, A, B, x[15], S23, K[22]); /* 23 */
        GG(B, C, D, A, x[4 ], S24, K[23]); /* 24 */
        GG(A, B, C, D, x[9 ], S21, K[24]); /* 25 */
        GG(D, A, B, C, x[14], S22, K[25]); /* 26 */
        GG(C, D, A, B, x[3 ], S23, K[26]); /* 27 */
        GG(B, C, D, A, x[8 ], S24, K[27]); /* 28 */
        GG(A, B, C, D, x[13], S21, K[28]); /* 29 */
        GG(D, A, B, C, x[2 ], S22, K[29]); /* 30 */
        GG(C, D, A, B, x[7 ], S23, K[30]); /* 31 */
        GG(B, C, D, A, x[12], S24, K[31]); /* 32 */

        /* Round 3 */
        HH(A, B, C, D, x[5 ], S31, K[32]); /* 33 */
        HH(D, A, B, C, x[8 ], S32, K[33]); /* 34 */
        HH(C, D, A, B, x[11], S33, K[34]); /* 35 */
        HH(B, C, D, A, x[14], S34, K[35]); /* 36 */
        HH(A, B, C, D, x[1 ], S31, K[36]); /* 37 */
        HH(D, A, B, C, x[4 ], S32, K[37]); /* 38 */
        HH(C, D, A, B, x[7 ], S33, K[38]); /* 39 */
        HH(B, C, D, A, x[10], S34, K[39]); /* 40 */
        HH(A, B, C, D, x[13], S31, K[40]); /* 41 */
        HH(D, A, B, C, x[0 ], S32, K[41]); /* 42 */
        HH(C, D, A, B, x[3 ], S33, K[42]); /* 43 */
        HH(B, C, D, A, x[6 ], S34, K[43]); /* 44 */
        HH(A, B, C, D, x[9 ], S31, K[44]); /* 45 */
        HH(D, A, B, C, x[12], S32, K[45]); /* 46 */
        HH(C, D, A, B, x[15], S33, K[46]); /* 47 */
        HH(B, C, D, A, x[2 ], S34, K[47]); /* 48 */

        /* Round 4 */
        II(A, B, C, D, x[0 ], S41, K[48]); /* 49 */
        II(D, A, B, C, x[7 ], S42, K[49]); /* 50 */
        II(C, D, A, B, x[14], S43, K[50]); /* 51 */
        II(B, C, D, A, x[5 ], S44, K[51]); /* 52 */
        II(A, B, C, D, x[12], S41, K[52]); /* 53 */
        II(D, A, B, C, x[3 ], S42, K[53]); /* 54 */
        II(C, D, A, B, x[10], S43, K[54]); /* 55 */
        II(B, C, D, A, x[1 ], S44, K[55]); /* 56 */
        II(A, B, C, D, x[8 ], S41, K[56]); /* 57 */
        II(D, A, B, C, x[15], S42, K[57]); /* 58 */
        II(C, D, A, B, x[6 ], S43, K[58]); /* 59 */
        II(B, C, D, A, x[13], S44, K[59]); /* 60 */
        II(A, B, C, D, x[4 ], S41, K[60]); /* 61 */
        II(D, A, B, C, x[11], S42, K[61]); /* 62 */
        II(C, D, A, B, x[2 ], S43, K[62]); /* 63 */
        II(B, C, D, A, x[9 ], S44, K[63]); /* 64 */

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
/* End of MD5 class implementation */

/* Класс односвязного списка 
    Реализуются функции:
    1)Добавление
    2)Поиск
    3)Удаление */

class List
{
private:
    /* Класс узла содержит данные(строка, её хеш), а также указатель на следующий узел */
    class Node /* Узел данных */
    {
        private:
            /* Data for this node */
            std::string str;
            std::string hash;

        public:
            Node *pNext; /* Указатель на следующий узел */
            Node(){}
            Node(std::string str_, Node *pNext_)
            {
                this->str = str_;
                this->hash = md5(str_);
                this->pNext = pNext_;
            }
            ~Node(){ delete &str; delete &hash; }

            void getData()
            {
                std::cout << "string : " << this->str << "\thash : " << this->hash << std::endl;
            }

            void setString(std::string str_){ this->str = str_; }
            std::string getString(){ return this->str; }

            /* Для инициализации */
            void setHash(std::string hash_){ this->hash = hash_; }
            std::string getHash(){ return this->hash; }
    };

    Node *head; /* Первый элемент */
    Node *tail; /* Последний элемент */
    size_t nodeCnt; /* Количество узлов */

public:
    List()
    {
        head = nullptr;
        nodeCnt = 0;
    }
    ~List()
    {
        Node *ptr = this->head;
        if(nodeCnt == 0)
        {
            free(head);
            delete head;
        }
        else
        {
            for (size_t i = 0; i < nodeCnt; i++)
            {
                free(ptr);
                ptr = ptr->pNext;
            }
        }
        free(ptr);
        delete ptr;
    }

    Node *getHead(){ return this->head; }
    size_t getSize() { return this->nodeCnt; }

    void addHash(std::string data)
    {
        Node *tmp = new Node();
        tmp->setString(data);
        tmp->setHash(md5(data));
        tmp->pNext = nullptr;

        if(head == nullptr)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->pNext = tmp;
            tail = tail->pNext;
        }
        nodeCnt++;
    }
    void searchHash(std::string hash_, Node *firstNode)
    {
        Node *ptr = firstNode;
        size_t i = 0;
        while(ptr != nullptr)
        {
            i++;
            if(ptr->getHash() == hash_) { std::cout << "Hash found on " << i << " index. String is '" << ptr->getString() << "'" << std::endl; break;}
            else
                ptr = ptr->pNext;
            if(ptr == nullptr) 
                std::cerr << "No proper hash found.\n";
        }
    }

    void deleteHash(size_t idx, Node *firstNode)
    {
        if(firstNode == nullptr) return;

        if(idx == 1) /* Удаляем head узел */
        {
            Node *newHead = firstNode->pNext;
            this->head = newHead;
            free(firstNode);
        }

        Node *current = firstNode;
        Node *prev = new Node;
        for(size_t i = 1; i < idx; i++)
        {
            prev = current;
            current = current->pNext;
        }
        prev->pNext = current->pNext;

        nodeCnt--;
    }

    void listAll(Node *firstNode)
    {   
        if(nodeCnt == 0)
        {
            std::cerr << "List is empty!\n";
        }
        else
        {
            Node *ptr = firstNode;
            size_t i = 0;
            while(ptr != nullptr)
            {
                i++;
                std::cout << "[" << i << "] "; ptr->getData(); 
                ptr = ptr->pNext;
            }
        }
    }
};

class Menu
{
private:
    friend class List;
    friend class MD5;

    List HashTable;
    std::string str;
    short choice;

public:
    Menu(){}
    ~Menu(){}

    void mainMenu()
    {
        std::cout << "MD5 hashing program.\nChoose an option: \n";
        std::cout << "[1] Make new hash. \n[2] Find string by hash.\n[3] List all hashes.\n[4] Delete hash from list.\n[5] Exit\n";
        std::cin >> choice; std::cout << std::endl;
        switch(choice)
        {
            case 1:
                makeNewHash();
                break;
            case 2:
                findStr();
                break;
            case 3:
                listAll();
                break;
            case 4:
                delHash();
                break;
            case 5:
                break;
            default:
                std::cout << "Incorrect input!" << std::endl;
                break;
        }
    }
    void makeNewHash()
    {
        CLEAR;
        std::cout << "Enter string to add : "; std::cin >> str;
        HashTable.addHash(str);
        std::cout << "Added!\n";
        mainMenu();
    }
    void findStr()
    {
        CLEAR;
        std::cout << "Enter hash to find : "; std::cin >> str;
        HashTable.searchHash(str, HashTable.getHead());
        mainMenu();
    }
    void listAll()
    {
        CLEAR;
        HashTable.listAll(HashTable.getHead());
        mainMenu();
    }
    void delHash()
    {
        CLEAR;
        size_t index;
        std::cout << "Enter index to delete : "; std::cin >> index;
        HashTable.deleteHash(index, HashTable.getHead());
        mainMenu();
    }
};

int main(int argc, char const* argv[])
{
    Menu menu;
    menu.mainMenu();
    return 0;
}