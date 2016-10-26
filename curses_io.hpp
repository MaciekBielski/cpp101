#ifndef CURSES_IO_HPP
#define CURSES_IO_HPP

#include <iostream>
#include <functional>
#include <ncurses.h>
#include "charset.hpp"

using namespace std;

extern const int CENTERY;
extern const int CENTERX;
extern const int DBGTOP;
extern const int DBGLEN;
extern const int DBGBOTTOM;

class CursesIO
{
    private: 
        bool validChar(char k) const;
        const function<char()> getc = nullptr;
        const function<void(char)> putc = nullptr;
        const CharSet &chSet;
        WINDOW *dbgw = nullptr;
    public:
        CursesIO() = delete;
        ~CursesIO();
        explicit CursesIO(const CharSet& charset);
        void clearScreen() const;
        const CursesIO& operator>>( char& c) const;
        void printc(const char c) const { this->putc(c); }
        void err(const string& str) const;
        void correctLast(const char c) const;
        /* conversion to bool */
        operator bool( ) const { return true; }
        inline const CharSet& getCharSet() const { return this->chSet; }
};

#endif /*CURSES_IO_HPP */
