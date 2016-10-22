/*
 * Simple calculator
 */

#include <iostream>
#include <sstream>
#include <string>
#include <ncurses.h>

#include "token.hpp"
#include "curses_io.hpp"

int main()
{
    CursesIO io{};
    io.clearScreen();
    Token::parseInput(io);

    return 0;
}

/******************************************************************************
 * Definitions
 */

const int NUMROWS {LINES/2}      ;
const int NUMCOLS {COLS/2}       ;
const int CENTERY {(NUMROWS) -1} ;
const int CENTERX {(NUMCOLS) -1} ;

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
