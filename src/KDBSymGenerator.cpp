#include <string>
#include <stdlib.h>
#include "KDBSymGenerator.hpp"

const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr auto SYMNR = 26 * 26 * 26 * 26;

KDBSymGenerator::KDBSymGenerator(unsigned long size) {
    symUniverse.resize(SYMNR);
    unsigned int i=0;
    for (auto a : alphabet)
        for (auto b : alphabet)
            for (auto c : alphabet)
                for (auto d : alphabet)
                    symUniverse[i++] = {a,b,c,d,'\0'};

    srand(time(nullptr));
    symList.reserve(size);
    for (i = 0; i < size; ++i) {
        symList.push_back(symUniverse[rand() % SYMNR].data());
    }

    sym_it = symList.begin();
}