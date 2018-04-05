//
// Created by nasty on 21.03.2018.
//

#include "ShannonFano.h"


std::string ShannonFano::findCode(char ch) {
    for (auto p: codes)
        if (p.first == ch)
            return p.second;
    return "";
}

int ShannonFano::findChar(std::string &code) {
    for (int i = 0; i< codes.size(); ++i)
        if (codes[i].second == code) {
            return i;
        }

    return -1;
}

void ShannonFano::encode(std::ofstream &out) {
    int pos = 0;
    char cur = ' ';
    std::string str;

    // посимвольно кодируем рабочий файл
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
                default:
                    break;
            }
        }
    }

    // считаем сколько битов в последнем файле не использовано
    int count = 1;

    while (pos != 8) {
        cur |= (1 << pos);
        ++pos;
        ++count;
    }

    out << cur;
    // записываем в новый файл (число неиспользованных битов + 1) как символ
    out << count;

}

void ShannonFano::createFile(std::string& path) {
    std::ofstream out(path + ".shan", std::ios::out | std::ios::binary);

    if (!out.is_open())
        throw std::ios_base::failure("file not open");

    WriteInt(out, (int)chances.size());

    for (auto it: chances) {
            out << it.first;
        WriteInt(out, it.second);
    }

    encode(out);

    out.close();
}

void ShannonFano::divide(int start, int last) {
    // количество букв в первой и второй группах соответственно
    int p1 = 0;
    int p2 = 0;

    int i = start;
    int j = last;

    while (i <= j)
        if (p1 < p2) {
            p1 += chances[i].second;
            ++i;
        } else {
            p2 += chances[j].second;
            --j;
        }

    for (int k = start; k <= j; ++k)
        codes[k].second += "0";
    for (int k = i; k <= last; ++k)
        codes[k].second += "1";

    if (j > start)
        divide(start, j);
    if (i < last)
        divide(i, last);
}

void ShannonFano::buildCodeTable() {
    if (chances.empty())
        throw std::logic_error("ProbTable was not build!");

    for (auto cur: chances)
        codes.emplace_back(std::pair<char, std::string>(cur.first, ""));

    divide(0, (int)chances.size() - 1);
}

/// функция сравнения двухчастот при сортировке
bool myfunction(std::pair<char, int> &i, std::pair<char, int> &j) { return (i.second > j.second); }

// TODO: ошибка через new  или нет?
void ShannonFano::buildProbTable() {
    if (mDataBuffer.empty())
        throw std::logic_error("Encoding data was not loaded!");

    // считаем частоты всех символов
    std::map<char, int> prechances;

    for (auto cur: mDataBuffer) {
        auto it = prechances.find(cur);
        if (it != prechances.end())
            ++it->second;
        else
            prechances.emplace_hint(prechances.end(), cur, 1);;
    }

    // сортруем частоты
    for (auto cur: prechances) {
        chances.emplace_back(cur);
    }

    std::sort(chances.begin(), chances.end(), myfunction);

}

void ShannonFano::loadFileInfo(std::string path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // вычисляем размер файла
    file.seekg(0, std::ios::end);
    long long sizef = file.tellg();
    file.seekg(0, std::ios::beg);

    // загружаем его полностью в буфер
    mDataBuffer.resize((unsigned long long) sizef);
    file.read((char *) (&mDataBuffer[0]), sizef);

    file.close();
}

void ShannonFano::loadInfo(std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // вычисляем изначальный размер файла
    file.seekg(0, std::ios::end);
    long long sizef = file.tellg();
    file.seekg(0, std::ios::beg);

    int count;
    ReadInt(file, count);

    // считываем таблицу частот
    for (int i = 0; i < count; ++i) {
        int prob;
        char sim;
        file.get(sim);
        ReadInt(file, prob);
            chances.emplace_back(std::pair<char, int>(sim, prob));
    }

    // вычисляем размер закодированной информации, которую необходимо считать
    // вычитаем 1 т.к. последний символ - это количество ненужных бит в последнем байте
    sizef = sizef - 1 - file.tellg();

    mDataBuffer.resize((unsigned long long) sizef);
    file.read((char *) (&mDataBuffer[0]), sizef);

    file >> r;
    --r;

    file.close();
}

void ShannonFano::decode(std::string& path) {
    std::ofstream out(path.insert(path.size() - 4, "un"), std::ios::out | std::ios::binary);

    if (!out.is_open())
        throw std::ios_base::failure("file not open");

    int pos = 0;
    int max = (int)mDataBuffer.size() * 8 - r;

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

void ShannonFano::clearAll() {
    chances.clear();
    codes.clear();
    mDataBuffer.clear();
}

void ShannonFano::pack(std::string& path) {
    clearAll();
    loadFileInfo(path);
    buildProbTable();
    buildCodeTable();
    createFile(path);
}

void ShannonFano::unpack(std::string& path) {
    clearAll();
    loadInfo(path);
    buildCodeTable();
    decode(path);
}

double  ShannonFano::getEntr() {
    double all = 0;
    for (auto cur : chances)
        all += cur.second;

    double h = 0;

    for (auto cur : chances)
        h += ((double)cur.second / all) * log2(((double)cur.second / all));

    return 0 - h;
}