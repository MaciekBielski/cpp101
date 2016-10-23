#include "token.hpp"

/*
 * CursesIO::operator>> performs already basic filtering so only characters
 * from predefined charset are returned at this level.
 *
 * Second-level filtering is that values are built from multiple characters and
 * operators are single-character. Also operators sometimes can invalidate
 * previous operators, assuming user made a mistake. Numbers have only care
 * about doubled period.
 */
static void emit(const CursesIO &io, ostringstream &acc)
{
    io.err(acc.str());
    acc.str("");
    acc.clear();
}

void Token::parseInput(const CursesIO& io)
{
    static ostringstream valAcc {};
    static ostringstream opAcc {};
    const CharSet& chSet = io.getCharSet();

    for(char c; io >> c;)
    {
        //TODO: finished here
        if( chSet.isVal(c) )
        {
            /* emit the operator if not empty */
            if( opAcc.rdbuf()->in_avail() > 0 )
                emit(io, opAcc);
        }
        else
        {
        }
        io.printc(c);
    }
}
