//
// Created by nasty on 23.03.2018.
//

#include "Huffman.h"

Huffman::Node::Node(Node *first, Node *second) {
    this->value = first->value + second->value;
    ch = '-';
    fistElement = first;
    secondElement = second;
}

Huffman::Node::~Node() {
    delete fistElement;
    delete secondElement;
}

Huffman::Node::Node(int value, int key, char ch) {
    this->value = value;
    this->ch = ch;
    fistElement = nullptr;
    secondElement = nullptr;
}

int Huffman::Node::compare(Node *another) {
    return this->value - another->value;
}

char Huffman::Node::getSimbol() {
    return ch;
}

int Huffman::Node::getProb() {
    return value;
}

Huffman::Node *Huffman::Node::getFirst() {
    return fistElement;
}

Huffman::Node *Huffman::Node::getSecond() {
    return secondElement;
}

Huffman::Node *Huffman::Node::unite(Node *first, Node *second) {
    return new Node(first, second);
}

/// функция сравнения двухчастот при сортировке
bool myFunction(std::pair<char, int> &i, std::pair<char, int> &j) { return (i.second > j.second); }

void Huffman::buildProbTable() {
    if (mDataBuffer.empty())
        throw std::logic_error("Encoding data was not loaded!");

    // строим таблицу частот встречаемости всех символов рабочего файла
    std::map<char, int> prechances;

    for (int i = 0; i < mDataBuffer.size(); ++i) {
        auto it = prechances.find(mDataBuffer[i]);
        if (it != prechances.end())
            ++it->second;
        else
            prechances.emplace_hint(prechances.end(), mDataBuffer[i], 1);;
    }

    // сортируем таблицу частот
    std::vector<std::pair<char, int>> preprechances;

    for (auto cur: prechances) {
        preprechances.emplace_back(cur);
    }

    size = (int) preprechances.size();

    std::sort(preprechances.begin(), preprechances.end(), myFunction);

    for (auto cur: preprechances) {
        chances.emplace_back(new Node(cur.second, chances.size(), cur.first));
    }
}

void Huffman::buildTree() {
    // берем 2 последние ноды и соединяем их общим родителем
    // до тех пор пока не останется одна корневая нода
    while (chances.size() > 1) {
        int j = (int) chances.size() - 1;
        while (j > 0 && chances[j]->compare(chances[j - 1]) > 0) {
            --j;
            Node *tmp = chances[j + 1];
            chances[j + 1] = chances[j];
            chances[j] = tmp;
        }

        Node *first = chances.back();
        chances.pop_back();
        Node *second = chances.back();
        chances.pop_back();

        Node *parent = Node::unite(first, second);
        chances.push_back(parent);
    }
}

void Huffman::buildCodes(Node *root, std::string code) {
    // добавляем нолик к коду левого потомка
    if (root->getFirst() != nullptr)
        buildCodes(root->getFirst(), code + "0");

    // добавляем единичку к коду правого потомка
    if (root->getSecond() != nullptr)
        buildCodes(root->getSecond(), code + "1");

    // добавляем в таблицу кодов, если дошли до листа
    if (root->getFirst() == nullptr && root->getSecond() == nullptr)
        codes.emplace_back(std::pair<char, std::string>(root->getSimbol(), code));

}

void Huffman::buildCodeTable() {
    // строим дерево зависимостей частот
    buildTree();

    std::string code = "";

    // строим таблицу кодов
    buildCodes(chances[0], code);
}

void Huffman::clearAll() {
    for (int i = 0; i < chances.size(); ++i)
        delete chances[0];

    chances.clear();
    codes.clear();
    mDataBuffer.clear();
}

void Huffman::loadFileInfo(std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // узнаем размер рабочего файла
    file.seekg(0, std::ios::end);
    long long sizef = file.tellg();
    file.seekg(0, std::ios::beg);

    mDataBuffer.resize((unsigned long long) sizef);
    file.read((char *) (&mDataBuffer[0]), sizef);

    file.close();
}

std::string Huffman::findCode(char ch) {
    for (auto p: codes)
        if (p.first == ch)
            return p.second;
    return "";
}

void Huffman::encode(std::ofstream &out) {
    int pos = 0;
    char cur = ' ';
    std::string str;

    // кодируем рабочий файл
    for (auto ch: mDataBuffer) {
        str = findCode(ch);
        for (auto bit: str) {
            if (pos == 8) {
                out << cur;
                pos = 0;
            }
            switch (bit) {
                case '0':
                    cur &= ~(1 << pos);
                    ++pos;
                    break;
                case '1':
                    cur |= (1 << pos);
                    ++pos;
                    break;
            }
        }
    }

    // считаем сколько битов в последнем байте остались незадействованными
    int count = 1;

    while (pos != 8) {
        cur |= (1 << pos);
        ++pos;
        ++count;
    }

    out << cur;
    // записываем количество незадействованных битов в файл как символ
    out << count;
}

void Huffman::createFile(std::string &path) {
    std::ofstream out(path + ".huff", std::ios::out | std::ios::binary);

    if (!out.is_open())
        throw std::ios_base::failure("file not open");

    // запись в итоговый файл таблицы частот
    WriteInt(out, size);
    for (int i = 0; i < size; ++i) {
        out << chances[i]->getSimbol();
        WriteInt(out, chances[i]->getProb());
    }

    // строим кодовую таблицу, уничтожая таблицу частот
    buildCodeTable();
    encode(out);

    out.close();
}

void Huffman::loadInfo(std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // считаем размер рабочего файла
    file.seekg(0, std::ios::end);
    long long sizef = file.tellg();
    file.seekg(0, std::ios::beg);

    // считываем таблицу частот
    int count;
    ReadInt(file, count);

    for (int i = 0; i < count; ++i) {
        int prob;
        char sim;
        file.get(sim);
        ReadInt(file, prob);
        chances.emplace_back(new Node(prob, (int) chances.size(), sim));
    }

    // вычисляем размер закодированной части файла
    sizef = sizef - 1 - file.tellg();

    mDataBuffer.resize((unsigned long long) sizef);
    file.read((char *) (&mDataBuffer[0]), sizef);

    // считываем количество незадействованных битов в последнем байте
    file >> r;
    --r;

    file.close();
}

int Huffman::findChar(std::string &code) {
    for (int i = 0; i < codes.size(); ++i)
        if (codes[i].second == code) {
            return i;
        }
    return -1;
}

void Huffman::decode(std::string &path) {
    std::ofstream out(path.insert(path.size() - 4, "un"), std::ios::out | std::ios::binary);

    if (!out.is_open())
        throw std::ios_base::failure("file not open");

    int pos = 0;
    int max = (int) mDataBuffer.size() * 8 - r;

    std::string cur = "";

    while (pos < max) {
        int n = pos % 8;
        int x = mDataBuffer[pos / 8];
        auto b = bool((1 << n) & x);
        cur += b ? '1' : '0';
        int i = findChar(cur);
        if (i != -1) {
            out << codes[i].first;
            cur = "";
        }
        ++pos;
    }
    out.close();
}


Huffman::~Huffman() {
    for (int i = 0; i < chances.size(); ++i)
        delete chances[0];
}

void Huffman::pack(std::string &path) {
    clearAll();
    loadFileInfo(path);
    buildProbTable();
    createFile(path);
}

void Huffman::unpack(std::string &path) {
    clearAll();
    loadInfo(path);
    buildCodeTable();
    decode(path);
}