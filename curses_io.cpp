#include "curses_io.hpp"

using namespace std;

CursesIO::CursesIO(const CharSet& charset):
    getc{getch}, putc{addch}, chSet{charset}
{
    wresize(stdscr, LINES-DBGTOP-1, COLS);
    dbgw = newwin(DBGLEN, COLS,DBGTOP,0);
}

CursesIO::~CursesIO()
{
    delwin(dbgw);
}

static void touchDefault()
{
    move(DBGTOP-2, 2);
    printw("= ");
    refresh();
}

void CursesIO::clearScreen() const
{
    clear();
    box(stdscr,0,0);
    refresh();
    curs_set('_');
    wclear(dbgw);
    box(dbgw,0,0);
    wrefresh(dbgw);
    touchDefault();
    //mvwprintw(dbgw,50,20,"TEST");
    //wrefresh(dbgw);
}

/* validate input characters wrt predefined sets */
bool CursesIO::validChar(char k) const
{
    auto out = false;
    if( chSet.isVal(k) || chSet.isAddOrSub(k) || chSet.isMulOrDiv(k) ||
            chSet.isBracket(k) )
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

void CursesIO::err(const string& str) const
{
    mvwprintw( dbgw, DBGTOP+1,1,str.c_str() );
    wrefresh(dbgw);
    touchDefault();
}
