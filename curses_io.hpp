#ifndef CURSES_IO_HPP
#define CURSES_IO_HPP

#include <iostream>
#include <functional>
#include <ncurses.h>
#include <set>
using namespace std;

extern const int NUMROWS  ;
extern const int NUMCOLS  ;
extern const int CENTERY  ;
extern const int CENTERX  ;

class CursesIO
{
    private: 
        bool validChar(char k) const;
        const function<char()> getc = nullptr;
        const function<void(char)> putc = nullptr;
    public:
        explicit CursesIO();
        void clearScreen() const;
        const CursesIO& operator>>( char& c) const;
        void printc(const char& c) const { this->putc(c); }
        /* conversion to bool */
        operator bool( ) const { return true; }
};

#endif /*CURSES_IO_HPP */
