#pragma once

#include <iostream>
#include <sstream>

#include "curses_io.hpp"

using namespace std;
using uint = unsigned int;

class TokenStream
{
    public:
        static void parseInput( const CursesIO& io);
};
