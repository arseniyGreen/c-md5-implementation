class List
{
private:
    /* Класс узла содержит данные(строка, её хеш), а также указатель на следующий узел */
    class Node /* Узел данных */
    {
        private:
            /* Данные для текущего узла */
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
        if(nodeCnt == 0)
        {
            delete head;
        }
        else
        {
            while(head->pNext != nullptr)
            {
                Node *ptr = head;
                for(size_t i = 0; i < nodeCnt - 1; ++i)
                {
                    ptr = ptr->pNext;
                    delete ptr;
                }
            }
            delete head;
        }
    }

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
    void searchHash(std::string hash_)
    {
        Node *ptr = head;
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

    void deleteHash(size_t idx)
    {
        if(head == nullptr) return;

        if(idx == 1) /* Удаляем head узел */
        {
            Node *newHead = head->pNext;
            this->head = newHead;
        }

        Node *current = head;
        Node *prev = new Node;
        for(size_t i = 1; i < idx; i++)
        {
            prev = current;
            current = current->pNext;
        }
        prev->pNext = current->pNext;

        nodeCnt--;
    }

    void listAll()
    {   
        if(nodeCnt == 0)
        {
            std::cerr << "List is empty!\n";
        }
        else
        {
            Node *ptr = head;
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