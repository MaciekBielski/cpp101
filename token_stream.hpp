#pragma once

#include <iostream>
#include <sstream>

#include "curses_io.hpp"
#include "token.hpp"

using namespace std;
using uint = unsigned int;

class TokenStream {
    private:
        Token emitToken(const CursesIO &io, stringstream &acc);
    public:
        void parseInput(const CursesIO& io) const;
};
