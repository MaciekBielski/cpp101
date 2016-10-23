#include "curses_io.hpp"

using namespace std;

CursesIO::CursesIO(const CharSet& charset):
    getc{getch}, putc{addch}, chSet{charset}
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
        chSet.vals.find(k) != chSet.vals.end() ||
        chSet.addSub.find(k) != chSet.addSub.end() ||
        chSet.mulDiv.find(k) != chSet.mulDiv.end() ||
        chSet.brackets.find(k) != chSet.brackets.end()
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
