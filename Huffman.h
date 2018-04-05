//
// Created by nasty on 23.03.2018.
//

#ifndef READBIN_HUFFMAN_H
#define READBIN_HUFFMAN_H


#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "FileIO.h"

/// упаковщик-распаковщик с алгоритмом Хаффмана
class Huffman {
private:
    /// узел представляющий узлы дерева Хаффмана для построения кодов
    class Node {
    private:
        /// левый предок ноды
        Node *fistElement;
        /// правый предок ноды
        Node *secondElement;
        /// частота встречаемости данного символа(символов)
        int value;
        /// символ, если нода является листом
        char ch;

        Node(Node *first, Node *second);

        Node(const Node &another) {}

        Node &operator=(const Node &another) {}

    public:
        ~Node();

        Node(int value, int key, char ch);

        int compare(Node *another);

        char getSimbol();

        int getProb();

        Node *getFirst();

        Node *getSecond();

        /// объединяет две ноды, порождая ноду-родителя
        static Node *unite(Node *first, Node *second);
    };

    /// таблица частот
    std::vector<Node *> chances;
    /// таблица кодов
    std::vector<std::pair<char, std::string>> codes;
    /// байтовое предсавление рабочего файла
    std::vector<unsigned char> mDataBuffer;
    /// количество различных символов рабочего файла
    int size;
    /// количество неиспользованных, но вписанных при упаковке битов
    int r;

    /// строит дерево зависимости частот символов
    void buildTree();

    /// строит коды в соответствии с деревом зависимости частот
    void buildCodes(Node *root, std::string code);

    Huffman(const Huffman &another) {}

    Huffman &operator=(const Huffman &another) {}

    /// строит словарь кодов
    void buildCodeTable();

    ///  очищает все поля класса
    void clearAll();

    /// загружает рабочий файл для упаковки
    void loadFileInfo(std::string &path);

    /// строит таблицу частот
    void buildProbTable();

    /// возвращает код символа
    std::string findCode(char ch);

    /// кодирует рабочий файл, заполняя новый файл с расширением .huff
    void encode(std::ofstream &out);

    /// создает файл соответствующий рабочему с расширением .huff
    void createFile(std::string& path);

    /// загружает рабочий  файл для распаковки
    void loadInfo(std::string& path);

    /// возвращает символ соответствующий коду
    int findChar(std::string &code);

    /// декодирует рабочий файл, заполняя новый файл с расширением .unhuff
    void decode(std::string& path);

public:
    Huffman() {}

    ~Huffman();

    /// упаковка файла path
    void pack(std::string& path);

    /// распаковка файла path
    void unpack(std::string& path);
};


#endif //READBIN_HUFFMAN_H
