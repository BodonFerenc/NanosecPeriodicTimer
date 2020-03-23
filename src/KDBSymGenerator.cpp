#include <stdlib.h>
#include "KDBSymGenerator.hpp"

constexpr auto SYMNR = 26 * 26 * 26 * 26;

KDBSymGenerator::KDBSymGenerator(unsigned long size) {  
    symUniverse.resize(SYMNR);
    unsigned int i=0;
    for (char a = 'A'; a <= 'Z'; ++a) {
        for (char b = 'A'; b <= 'Z'; ++b) {
            for (char c = 'A'; c <= 'Z'; ++c) {
                for (char d = 'A'; d <= 'Z'; ++d) {
                    symUniverse[i][0] = a;
                    symUniverse[i][1] = b;
                    symUniverse[i][2] = c;
                    symUniverse[i][3] = d;
                    symUniverse[i][4] = '\0';
                    ++i;
                }
            }
        }
    }

    srand(time(nullptr));
    symList.reserve(size);
    for (i = 0; i < size; ++i) {
        symList.push_back(symUniverse[rand() % SYMNR].data());
    }

    sym_it = symList.begin();
}