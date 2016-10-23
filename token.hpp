#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <iostream>
#include <sstream>
#include "curses_io.hpp"
using namespace std;

class Token
{
    public:
        static void parseInput( const CursesIO& io);
};

#endif /* TOKEN_HPP */
