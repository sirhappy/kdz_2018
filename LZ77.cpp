//
// Created by nasty on 24.03.2018.
//

#include "LZ77.h"

void LZ77::loadInfo(std::string& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // считываем код
    int offs, len;
    char ch;

    ReadInt(file, offs);
    while (file.get(ch)) {
        ReadInt(file, len);
        res.emplace_back(Node(offs, len, ch));
        ReadInt(file, offs);
    }
    file.close();
}

LZ77::Node LZ77::findMaxSubstring(std::string& dictionary, std::string& window) {
    // минимально возможный код для вставки
    Node n(0, 0, window[0]);

    size_t winLen = window.length() - 1;
    size_t dictLen = dictionary.length();

    // пробегаем по всем возможным подстрокам из окна
    for (int i = 1; i <= std::min(winLen, dictLen); i++)
    {
        // формируем подстроку нужной длины
        std::string s = window.substr(0, i);

        size_t pos = dictionary.find(s);
        if (pos == std::string::npos)
            break;

        if ((dictionary.compare(dictLen - i, i, s, 0, i) == 0) && (window[0] == window[i]))
            pos = dictLen - i;

        // дополнительная длина за счёт повторов
        int extra = 0;
        if (dictLen == pos + i)
        {
            // считаем целые повторения
            while ((winLen >= i + extra + i) && (window.compare(i + extra, i, s, 0, i) == 0))
                extra += i;

            // смотрим не повторяется ли часть строки
            int extraextra = i - 1;
            while (extraextra > 0)
            {
                if ((winLen >= i + extra + extraextra) && (window.compare(i + extra, extraextra, s, 0, extraextra) == 0))
                    break;
                extraextra--;
            }

            extra += extraextra;
        }

        // сравниваем длины предыдущего максимального узла и текущего
        if (n.len <= i + extra)
            n = Node(dictLen - pos, i + extra, window[i + extra]);
    }

    return n;
}

void LZ77::encode() {
    std::string dictionary = "";
    std::string window = "";

    while (true) {
        if ((window.length() < winBufMax) && (mDataBuffer.length() != 0)) {
            int len = winBufMax - (int)window.length();
            window.append(mDataBuffer, 0, len);
            mDataBuffer.erase(0, len);
        }

        if (window.length() == 0)
            break;

        Node n = findMaxSubstring(dictionary, window);

        // закодированные данные перемещаются из окна в словарь
        dictionary.append(window, 0, n.len + 1);
        window.erase(0, n.len + 1);

        if (dictionary.length() > dictBufMax)
            dictionary.erase(0, dictionary.length() - dictBufMax);

        res.push_back(n);
    }
}

void LZ77::decode() {
    for (auto cur : res) {
        int len = cur.len;
        if (len > 0) {
            std::string s = mDataBuffer.substr(mDataBuffer.length() - cur.offs, std::min(len, cur.offs));
            // учитываем возможне повторения
            while (len) {
                int extra = std::min(len, (int) s.length());
                mDataBuffer.append(s, 0, extra);
                len -= extra;
            }
        }

        mDataBuffer.append(1, cur.ch);
    }
}

void LZ77::loadFileInfo(std::string &path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);

    if (!file.is_open())
        throw std::ios_base::failure("file not open");

    // считаем  длину рабочего файла
    file.seekg(0, std::ios::end);
    long long sizef = file.tellg();
    file.seekg(0, std::ios::beg);

    // считываем рабчий файл в буфер
    mDataBuffer.resize((unsigned long long) sizef);
    file.read((&mDataBuffer[0]), sizef);

    file.close();
}

void LZ77::createFile(std::string& path) {
        std::string newPath = "" + path;
        switch (winBufMax) {
            case 5*1024: newPath += ".lz7705"; break;
            case 10*1024: newPath += ".lz7710"; break;
            case 20*1024: newPath += ".lz7720"; break;
            default: newPath += ".lz77";
        }

    std::ofstream out(newPath, std::ios::out | std::ios::binary);

    if (!out.is_open())
        throw std::ios_base::failure("file not open");

    // (offs, ch, len)
    for (auto cur: res) {
        WriteInt(out, cur.offs);

//        if (cur.ch == '\0') {
//            out << '=';
//            WriteInt(out, 0 - cur.len);
//        }
//        else {
            out << cur.ch;
            WriteInt(out, cur.len);
//        }

    }

    out.close();
}

void LZ77::createUnFile(std::string& path) {
    std::ofstream out(path.insert(path.size() - 6, "un"), std::ios::out | std::ios::binary);

    out << mDataBuffer;

    out.close();
}

void LZ77::clearAll() {
    res.clear();
    mDataBuffer.clear();
}


void LZ77::pack(std::string& path) {
    clearAll();
    loadFileInfo(path);
    encode();
    createFile(path);
}

void LZ77::unpack(std::string& path) {
    clearAll();
    loadInfo(path);
    decode();
    createUnFile(path);
}