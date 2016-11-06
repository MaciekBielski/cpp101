#include "curses_io.hpp"

using namespace std;

/* debug window is below stdscr and has a pan behind for scrolling emulation,
 * input window is nested within stdscr
 */
CursesIO::CursesIO(const CharSet& charset):
    getc{getch}, putc{waddch}, chSet{charset}
{
    wresize(stdscr, LINES-1-DBGTOP, COLS);
    in = newwin(INLINES, COLS-4, DBGTOP-1-INLINES, 2);
    dbgw = newwin(DBGLEN, COLS,DBGTOP,0);
    pad = newpad(PADLEN, COLS-4);
}

CursesIO::~CursesIO()
{
    delwin(in);
    delwin(dbgw);
}

void CursesIO::clearScreen() const
{
    clear();
    box(stdscr,0,0);
    refresh();
    wclear(dbgw);
    box(dbgw,0,0);
    wrefresh(dbgw);
    wprintw(in,"= ");
    wrefresh(in);
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

/* Moves the cursor to previous position, removes last charcter under the
 * cursor and replaces with c */
void CursesIO::correctLast(const char * const c) const
{
    int currY {0}, currX {0};
    getyx(in, currY, currX);
    if(currX == 0)
        wmove(in, currY-1, COLS-4-1);
    else
        wmove(in, currY, currX-1);
    wclrtoeol(in);
    if(c)
        this->putc(in, *c);
    wrefresh(in);
}

void CursesIO::acceptChar(const char c, stringstream &acc) const
{
    acc << c;
    this->putc(in, c);
    wrefresh(in);
}

/*
 * After all new window is just for the box, PAD is relative to screen
 * coordinates
 */
void CursesIO::err(const string& str) const
{
    static uint16_t padCurr = 0;
    static uint16_t padTop = 0;

    mvwprintw(pad, padCurr, 0, str.c_str() );
    prefresh(pad, padTop, 0, DBGFIRST, 2, DBGLAST, COLS-2 );
    padCurr = (padCurr +1) % PADLEN;
    if(0 == padCurr)
        wclear(pad);
    padTop = (padCurr > DBGINNER) ? padCurr - DBGINNER : 0 ;
    wrefresh(in);
}
