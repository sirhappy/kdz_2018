//
// Created by nasty on 25.03.2018.
//

#include "FileIO.h"

void ReadInt(std::istream &lv, int &rv) {
    const size_t nBytes = 4;
    char bytes[nBytes];
    for(int i = 0; i < (int)nBytes; ++i) {
        lv.get(bytes[i]);
    }
    rv = *static_cast<int*>(static_cast<void*>(bytes));

}

void WriteInt(std::ostream &lv, int rv) {
    lv.write(static_cast<char*>(static_cast<void*>(&rv)), sizeof(int));
}