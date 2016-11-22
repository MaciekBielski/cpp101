/*
 * Simple calculator
 *
 * TODO:
 * [-] threads
 * [-] I/O of Tokens
 * [-] emiting tokens in one thread, consuming in the other
 * [-] consuming as an iterator
 * [-] refactor
 * [-] issue with negative values
 * [-] singleton getInstance() -> initializer_list
 */

#include <iostream>
#include <ncurses.h>

#include "token_stream.hpp"
#include "curses_io.hpp"
#include "charset.hpp"

/* Globals */

const int CENTERY   {(LINES/2) -1} ;
const int CENTERX   {(COLS/2) -1} ;
const int DBGTOP    {CENTERY+1} ;
const int DBGLEN    {LINES-DBGTOP-1} ;
const int DBGINNER  {DBGLEN-3} ;
const int PADLEN    {DBGINNER*3};
const int DBGFIRST  {DBGTOP + 1};
const int DBGLAST   {DBGTOP + DBGINNER + 1};
const int INLINES   { static_cast<int>((LINES-DBGTOP)/4.0) };
/*
 * Main
 */
int main()
{
    CursesIO io{ CharSet::getInstance() };
    io.clearScreen();
    TokenStream::parseInput(io);

    return 0;
}

__attribute__((constructor)) static void screenInit()
{
    initscr();
    cbreak();
    noecho();       //don't print getch() character
}

__attribute__((destructor)) static void screenFinish()
{
    endwin();
}
