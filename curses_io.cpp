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
    //mvwprintw(dbgw,1, 2,"TEST");
    //wrefresh(dbgw);
}

/* Validate input characters wrt predefined sets */
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

/* Removes last charcter under the cursor and replaces with c */
void CursesIO::correctLast(const char c) const
{
    delch();
    this->putc(c);
}

void CursesIO::acceptChar(const char c, stringstream &acc) const
{
    acc << c;
    this->putc(c);
    err(string{ c });
}

/* This needs to be worked on */
void CursesIO::err(const string& str) const
{
    static int8_t errOffset = DBGBOTTOM;

    mvwprintw( dbgw, errOffset, 2,str.c_str() );
    wrefresh(dbgw);
    errOffset = ( (errOffset +1 - DBGBOTTOM) % (DBGLEN - 1 - DBGBOTTOM)) + DBGBOTTOM;

    //touchDefault();
}
