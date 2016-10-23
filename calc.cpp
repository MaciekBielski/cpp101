/*
 * Simple calculator
 */

#include <iostream>
#include <ncurses.h>

#include "token.hpp"
#include "curses_io.hpp"
#include "charset.hpp"

/* Globals */

const int CENTERY {(LINES/2) -1} ;
const int CENTERX {(COLS/2) -1} ;

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
    noecho();       //don't print getch() character
    curs_set('_');
}

__attribute__((destructor)) static void screenFinish()
{
    endwin();
}
