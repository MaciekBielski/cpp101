/*
 * Simple calculator
 *
 * TODO:
 * [-] brackets counting is not perfect
 * [-] describe synchronisation in parseInput
 * [-] issue with negative values
 */

#include <iostream>
#include <functional>
#include <memory>
#include <ncurses.h>
#include <thread>
#include <stack>

#include "curses_io.hpp"
#include "charset.hpp"
#include "token_stream.hpp"

/* Globals to setup screen layout */

const int CENTERY   {(LINES/2) -1} ;
const int CENTERX   {(COLS/2) -1} ;
const int DBGTOP	{CENTERY+1} ;
const int DBGLEN	{LINES-DBGTOP-1} ;
const int DBGINNER  {DBGLEN-3} ;
const int PADLEN	{DBGINNER*3};
const int DBGFIRST  {DBGTOP + 1};
const int DBGLAST   {DBGTOP + DBGINNER + 1};
const int INLINES   { static_cast<int>((LINES-DBGTOP)/4.0) };


/*
 * Input is parsed in a separate thread and an expression is built in the main
 * thread, parsing thread produces tokens out of allowed symbols and they are
 * passed to Expression (or Term) for further processing.
 */
int main()
{
	TokenStream ts{};	   //non-copyable, non-movable
	auto io = CursesIO{};

	io.setupWindows();
	io.clearScreen();

	auto uiThread = thread{[&ts, &io]() {ts.parseInput(io);} };

	auto filo = stack<unique_ptr<Token>>{};
	auto exp = Expression{filo, ts, io};

	/* When this returns it is a final result */
	exp.run();
	io.err("Result: "s + static_cast<string>(*(filo.top().release())));
	io.waitChar();

	uiThread.join();
	return 0;
}

__attribute__((constructor)) static void screenInit()
{
	initscr();
	cbreak();
	noecho();	   //don't print getch() character
}

__attribute__((destructor)) static void screenFinish()
{
	endwin();
}
