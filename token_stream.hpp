#ifndef TOKENSTREAM_HPP
#define TOKENSTREAM_HPP

#include <iostream>
#include <sstream>
#include "curses_io.hpp"
using namespace std;

class TokenStream
{
    public:
        static void parseInput( const CursesIO& io);
};

#endif
