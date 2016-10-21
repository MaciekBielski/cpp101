/*
 * Simple calculator
 */

#include <iostream>
#include <sstream>
#include <ncurses.h>
#include <set>
#include <string>

const static size_t NUMROWS (LINES/2)      ;
const static size_t NUMCOLS (COLS/2)       ;
const static size_t CENTERY ((NUMROWS) -1) ;
const static size_t CENTERX ((NUMCOLS) -1) ;

using namespace std;

static string valueChars {"0123456789."};
static string op1Chars {"+-"};
static string op2Chars {"*/"};
static set<char> valueSet { valueChars.cbegin(), valueChars.cend() };
static set<char> op1Set { op1Chars.cbegin(), op1Chars.cend() };
static set<char> op2Set { op2Chars.cbegin(), op2Chars.cend() };

/* validate input characters wrt predefined sets */
auto validChar(char k) -> bool {
    auto out = false;
    if( valueSet.find(k) != valueSet.end() ||
        op1Set.find(k) != op1Set.end() ||
        op2Set.find(k) != op2Set.end()
            )
        out = true;
    return out;
}

/* Gets characters one by one, return on enter, input passed immediately */
auto parseToken() -> void {
    for (char tmp; (tmp=getch()) && tmp!='\n' ;) {
        if(!validChar( tmp ))
            continue;
        addch(tmp);
    }
}


auto main() -> int {
    move(CENTERY, 1);
    printw("=");
    parseToken();
}

__attribute__((constructor)) static void screenInit()
{
    initscr();
    clear();
    noecho();       //don't print getch() character
    curs_set('_');    //hide the cursor
}

__attribute__((destructor)) static void screenFinish()
{
    endwin();
}
