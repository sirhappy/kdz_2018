// КДЗ по дисциплине Алгоритмы и структуры данных 2017-2018 уч.год
// Казанцева Анастасия Романовна, группа БПИ-162, дата (26.03.2018)
// CLion: FileIO.cpp, Huffman.cpp, LZ77.cpp, ShannonFano.cpp
//        FileIO.h, Huffman.h, LZ77.h, ShannonFano.h
// Реалтзованы алгоритмы упаковки и распаковки (Хаффман, Шеннон-Фано, LZ77)
// начаты эксперемент и отчет

#include <regex>
#include "ShannonFano.h"
#include "Huffman.h"
#include "LZ77.h"
#include <chrono>

std::string names[36] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
                         "11", "12", "13", "24", "15", "16", "17", "18", "19", "20",
                                 "21", "22", "23", "14", "25", "26", "27", "28", "29", "30",
                                 "31", "32", "33", "34", "35", "36"};

int main(int argc, char *argv[]) {
    for (auto path: names) {
        std::string unpath = path + ".huff";

        std::ifstream f(path);
        f.seekg(0, std::ios::end);
        long long sizef = f.tellg();
        f.close();

        std::ifstream s(unpath);
        s.seekg(0, std::ios::end);
        long long sizes = s.tellg();

        std::cout << path << ": " << (double) sizes / (double) sizef << '\n';
        s.close();
    }
    return 0;

}