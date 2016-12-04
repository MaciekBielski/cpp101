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
#include <memory>
#include <ncurses.h>
#include <thread>
#include <stack>

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


//TODO
//  unique_ptr<Token> getTerm (TokenStream &ts, const CursesIO &io,
//          stack<unique_ptr<Token>> &filo)

/* Returns with last computed value on the stack */
void getExpression (TokenStream &ts, const CursesIO &io,
        stack<unique_ptr<Token>> &filo)
{
    // stack initialization
    filo.push( make_unique<ValToken>("0"s) );
    filo.push( make_unique<AddSubToken>("+"s) );

    do {
        auto token = ts.passToken(io);
        //bool shouldReduce = false;

        io.err("Passed: "s + static_cast<string>(*token));

        /*
         * - compute returns true if reduction on current level should be done,
         *   after returning from nested level,
         *
         * - for ValToken compute pushes value on stack and returns false
         *
         * - for AddSubToken compute reduces last triple [rval, op, lval] on the
         *   stack and pushes the result, then pushes the operator and returns
         *   false:
         *
         * - for MulDivToken compute calls getTerm that operates till next
         *   AddSubToken or =), then it pushes reduced subresult on the stack,
         *   pushes finishing token and returns true,
         *   - then getExpression pops the token and reduces again last triple
         *   according to it, it will be AddSubToken or =)
         *
         * - for BracketToken compute does:
         *   - for ')' should reduce last triple, discard '(' that prepends
         *   this triple, push reduced value and return false, does not push
         *   ')'!
         *   - for '(' pushes it on the stack and calls getExpression that
         *   finishes with false and result on the stack
         *
         * - for '=' it reduces last triple, push the reduced value and returns
         *   false, does not push '='
         */
        while (true) {
            if (token.compute(ts,io,filo))
                token = filo.pop();
            else
                break;
        }
        auto lastToken = static_cast<string>(*token);

    } while ( (lastToken != "="s) && (lastToken != "="s) );
}

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

    auto filo = stack<unique_ptr<Token>>{};
    getExpression(ts, io, filo);

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
