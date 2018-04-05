//
// Created by nasty on 24.03.2018.
//

#ifndef READBIN_LZ77_H
#define READBIN_LZ77_H

#include <iostream>
#include <vector>
#include <fstream>
#include "FileIO.h"

/// упаковщик-распаковщик с алгоритмом Шенона-Фано
class LZ77 {
    /// <offs,len,ch> - блок кода
    class Node {
    public:
        /// смещение относительно начала словаря
        int offs;
        /// длина
        int len;
        /// первый незнакомый символ
        char ch;

        Node(int o, int l, char c) : offs(o), len(l), ch(c) {};

        Node() {};

    };

    /// байтовое предсавление рабочего файла
    std::string mDataBuffer;
    /// максимальный размер словаря
    int dictBufMax;
    /// максимальный размер окна предпросмотра
    int winBufMax;
    /// код
    std::vector<Node> res;

    /// ищет максимальную подстроку из окна в словаре+окне
    Node findMaxSubstring(std::string &dictionary, std::string &window);

    /// кодирует рабочий файл
    void encode();

    /// декодирует рабочий файл
    void decode();

    /// загружает рабочий файл для упаковки
    void loadFileInfo(std::string &path);

    /// создает файл с расширением .lz77** и упаковывает в него рабочий файл
    void createFile(std::string& path);

    /// создает файл с расширением .unlz77** и распаковывает в него рабочий файл
    void createUnFile(std::string& path);

    ///  очищает все поля класса
    void clearAll();

    /// загружает рабочий  файл для распаковки
    void loadInfo(std::string& path);

public:
    LZ77(int winBufMax, int dictBufMax) : dictBufMax(dictBufMax), winBufMax(winBufMax) {};

    /// упаковка файла path
    void pack(std::string& path);

    /// распаковка файла path
    void unpack(std::string& path);
};

#endif //READBIN_LZ77_H
