#include <iostream>
#include <string>

int HashFunctionHorner(const std::string &s, int table_size, const int key) //алгортим хеширования (по Горнеру)
{
    int hash_result = 0;
    for (int i = 0; i != s.size(); ++i)
        hash_result = (hash_result * key + s[i]) % table_size;
    hash_result = (hash_result * 2 + 1) % table_size;
    return hash_result;
}
//Для избежания коллизии используем двойное хеширование
struct HashFunction1
{
    int operator() (const std::string &s, int table_size) const
    {
        return HashFunctionHorner(s, table_size, table_size - 1);
    }
};

struct HashFunction2
{
    int operator() (const std::string& s, int table_size) const
    {
        return HashFunctionHorner(s, table_size, table_size + 1);
    }
};

template <class T, class THash1 = HashFunction1, class THash2 = HashFunction2>

class HashTable 
{
    static const int default_size = 8; //начальный размер хеш-таблицы
    constexpr static const int rehash = 0.75; //коэффицент, при котором происходит увелечение таблицы
    struct Node 
    {
        T value;
        bool state; //флаг для отслеживания удалённых элементов массива. Если == false, то элемент был удалён (deleted)
        Node(const T &value_) : value(value_), state(true) {}
    };
    Node** arr; //Массив, в котором будут хранится структуры Node*
    int size; //размерность массива без учёта deleted
    int buffer_size; //размер памяти, выделенной под хранение таблицы
    int size_all_non_nullptr; //размерность массива с учётом deleted
};

int main()
{
    std::cout << "I'm alive!\n";
}