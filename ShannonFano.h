//
// Created by nasty on 21.03.2018.
//

#ifndef READBIN_SHANNONFANO_H
#define READBIN_SHANNONFANO_H

#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "FileIO.h"
#include <math.h>       /* log2 */

/// упаковщик-распаковщик с алгоритмом Шенона-Фано
class ShannonFano {
private:
    /// таблица частот встречаемости символов рабочего файла
    std::vector<std::pair<char, int>> chances;
    /// таблица кодов
    std::vector<std::pair<char, std::string>> codes;
    /// байтовое предсавление рабочего файла
    std::vector<unsigned char> mDataBuffer;
    /// количество неиспользованных, но вписанных при упаковке битов
    int r;

    /// возвращает код символа
    std::string findCode(char ch);

    /// возвращает символ соответствующий коду
    int findChar(std::string &code);

    /// кодирует рабочий файл, заполняя новый файл с расширением .shan
    void encode(std::ofstream& out);

    /// создает файл соответствующий рабочему с расширением .shan
    void createFile(std::string& path);

    /// делит таблицу частот пополам при составлении кодовой таблица
    void divide(int start, int last);

    /// строит таблицу кодов
    void buildCodeTable();

    /// строит таблицу частот
    void buildProbTable();

    /// загружает рабочий файл для упаковки
    void loadFileInfo(std::string path);

    /// загружает рабочий  файл для распаковки
    void loadInfo(std::string& path);

    /// декодирует рабочий файл, заполняя новый файл с расширением .unshan
    void decode(std::string& path);

    /// очищает все поля класса
    void clearAll();

public:

    double getEntr();

    /// упаковка файла path
    void pack(std::string& path);

    /// распаковка файла path
    void unpack(std::string& path);
};


#endif //READBIN_SHANNONFANO_H