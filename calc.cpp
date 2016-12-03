/*
 * Simple calculator
 *
 * TODO:
 * [-] emitToken synchronize consumer & producer
 * [-] I/O of Tokens: Operator vs Operands
 * [-] emiting tokens in one thread, consuming in the other
 * [-] consuming as an iterator
 * [-] refactor
 * [-] issue with negative values
 */

#include <iostream>
#include <functional>
#include <ncurses.h>
#include <thread>

#include "curses_io.hpp"
#include "charset.hpp"
#include "token_stream.hpp"

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
    TokenStream ts{};       //non-copyable, non-movable
    auto io = CursesIO{};
    io.setupWindows();
    io.clearScreen();

    auto uiThread = thread{ [&ts, &io](){ ts.parseInput(io); } };

    for( ;;ts.passToken(io) );

    uiThread.join();
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
