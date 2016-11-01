/*
 * Simple calculator
 *
 * TODO:
 * [-] debug console
 * [-] printing double characters and cleaning line is fucked up
 * [-] refactor
 * [-] emiting token
 */

#include <iostream>
#include <ncurses.h>

#include "token.hpp"
#include "curses_io.hpp"
#include "charset.hpp"

/* Globals */

const int CENTERY    {(LINES/2) -1} ;
const int CENTERX    {(COLS/2) -1} ;
const int DBGTOP     {CENTERY+1} ;
const int DBGLEN     {LINES-DBGTOP-1} ;
const int DBGBOTTOM  {DBGTOP+DBGLEN} ;
/*
 * Main
 */
int main()
{
    CursesIO io{ CharSet::getInstance() };
    io.clearScreen();
    Token::parseInput(io);

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
