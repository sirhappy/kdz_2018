//
// Created by nasty on 24.03.2018.
//

#ifndef READBIN_FILEIO_H
#define READBIN_FILEIO_H

#include<iostream>
#include<fstream>

/// читает int из файла
void ReadInt(std::istream& lv, int& rv);
/// пишет int в файл
void WriteInt(std::ostream& lv, int rv);

#endif //READBIN_FILEIO_H
