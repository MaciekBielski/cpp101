#pragma once

#include <functional>
#include <iostream>
#include <ncurses.h>
#include <sstream>

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
extern const int INLINES;

/* non-copyable type */
class CursesIO
{
    private: 
        bool validChar(char k) const;
        const function<char()> getc = nullptr;
        const function<void(WINDOW *, char)> putc = nullptr;
        WINDOW *in = nullptr;
        WINDOW *dbgw = nullptr;
        WINDOW *pad = nullptr;
        const CharSet chSet;
    public:
        CursesIO();
        CursesIO(const CursesIO& rhs) = delete;
        CursesIO(CursesIO &&rhs) noexcept;
        CursesIO& operator=( CursesIO rhs ) = delete;
        virtual ~CursesIO();

        void clearScreen() const;
        void setupWindows();
        const CursesIO& operator>>( char& c) const;
        void acceptChar(const char c, stringstream &acc) const;
        void err(const string& str) const;
        void correctLast(const char * const c = nullptr) const;
        /* conversion to bool */
        operator bool( ) const { return true; }
        const CharSet& getCharSet() const { return this->chSet; }
};
