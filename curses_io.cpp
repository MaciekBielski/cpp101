#include "curses_io.hpp"

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

CursesIO::CursesIO():
    getc{getch}, putc{addch}
{}

void CursesIO::clearScreen() const
{
    clear();
    move(CENTERY, 1);
    printw("=");
}

/* validate input characters wrt predefined sets */
bool CursesIO::validChar(char k) const
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

/*
 * First level of filtering, returns a char only if belongs to predefined sets
 */
const CursesIO& CursesIO::operator>>( char& c ) const
{
    char tmp {0};
    do{
        tmp = this->getc();
    }while( !validChar(tmp));
    c = tmp;

    return *this;
}
