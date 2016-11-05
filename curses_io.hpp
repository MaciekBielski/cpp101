#ifndef CURSES_IO_HPP
#define CURSES_IO_HPP

#include <iostream>
#include <functional>
#include <sstream>
#include <ncurses.h>
#include "charset.hpp"

using namespace std;

extern const int CENTERY;
extern const int CENTERX;
extern const int DBGTOP;
extern const int DBGLEN;
extern const int PADLEN;
extern const int DBGFIRST;
extern const int DBGINNER;
extern const int DBGLAST;

class CursesIO
{
    private: 
        bool validChar(char k) const;
        const function<char()> getc = nullptr;
        const function<void(char)> putc = nullptr;
        const CharSet &chSet;
        WINDOW *dbgw = nullptr;
        WINDOW *pad = nullptr;
    public:
        CursesIO() = delete;
        explicit CursesIO(const CharSet& charset);
        virtual ~CursesIO();
        void clearScreen() const;
        const CursesIO& operator>>( char& c) const;
        void acceptChar(const char c, stringstream &acc) const;
        void err(const string& str) const;
        void correctLast(const char c) const;
        /* conversion to bool */
        operator bool( ) const { return true; }
        inline const CharSet& getCharSet() const { return this->chSet; }
};

#endif /*CURSES_IO_HPP */
