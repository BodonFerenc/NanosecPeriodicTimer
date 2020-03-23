/**
 * \class KDBSymGenerator
 *
 *
 * \brief Class to generate set of kdb+ symbols
 *
 * It returns random strings of length four consists of capital letters
 *
 * \author Ferenc Bodon
 *
 */

#pragma once

#include <vector>
#include <array>


class KDBSymGenerator {
    private:  
        std::vector<std::array<char, 5>> symUniverse;
        std::vector<char*> symList;        

    public: 
        std::vector<char*>::iterator sym_it;    
        KDBSymGenerator(unsigned long);
};
