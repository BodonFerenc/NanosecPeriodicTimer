#include <string>
#include <cstdlib>
#include <random>
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

    std::uniform_int_distribution<> dis(0, SYMNR-1);
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd());


    symList.reserve(size);
    for (i = 0; i < size; ++i) {
        symList.push_back(symUniverse[dis(gen)].data());
    }

    sym_it = symList.begin();
}