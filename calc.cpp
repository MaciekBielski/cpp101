/*
 * Simple calculator
 */

#include <iostream>
#include <sstream>
#include <ncurses.h>
#include <set>
#include <string>
#include <functional>

const static size_t NUMROWS (LINES/2)      ;
const static size_t NUMCOLS (COLS/2)       ;
const static size_t CENTERY ((NUMROWS) -1) ;
const static size_t CENTERX ((NUMCOLS) -1) ;

using namespace std;


struct charSet
{
    string valueChars  = {"0123456789."};
    string op1Chars    = {"+-"};
    string op2Chars    = {"*/"};
    string op3Chars    = {"()"};
    set<char> vals { valueChars.cbegin(), valueChars.cend() };
    set<char> addSub { op1Chars.cbegin(), op1Chars.cend() };
    set<char> mulDiv { op2Chars.cbegin(), op2Chars.cend() };
    set<char> brackets { op3Chars.cbegin(), op3Chars.cend() };
} const charSet;

class CursesIO
{
    private: 
        auto validChar(char k) const -> bool;

        const std::function<char()> getc = nullptr;
        const std::function<void(char)> putc = nullptr;
    public:
        explicit CursesIO();
        auto clearScreen() const -> void;
        auto parseToken() const -> void;
};

auto main() -> int
{
    CursesIO io{};
    io.clearScreen();
    io.parseToken();
}

/*
 * definitions
 */
CursesIO::CursesIO():
    getc{getch}, putc{addch}
{}

auto CursesIO::clearScreen() const -> void
{
    clear();
    move(CENTERY, 1);
    printw("=");
}

/* Gets characters one by one, returns on enter, input passed immediately */
auto CursesIO::parseToken() const -> void
{
    for (char tmp; (tmp=this->getc()) && tmp!='\n' ;) {
        if(!validChar( tmp ))
            continue;
        this->putc(tmp);
    }
}

/* validate input characters wrt predefined sets */
auto CursesIO::validChar(char k) const -> bool
{
    auto out = false;
    if(
        charSet.vals.find(k) != charSet.vals.end() ||
        charSet.addSub.find(k) != charSet.addSub.end() ||
        charSet.mulDiv.find(k) != charSet.mulDiv.end() ||
        charSet.brackets.find(k) != charSet.brackets.end()
        )
        out = true;
    return out;
}



__attribute__((constructor)) static void screenInit()
{
    initscr();
    noecho();       //don't print getch() character
    curs_set('_');
}

__attribute__((destructor)) static void screenFinish()
{
    endwin();
}
