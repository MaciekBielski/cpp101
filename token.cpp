#include "token.hpp"

/*
 * CursesIO::operator>> performs already basic filtering so only characters
 * from predefined charset are returned at this level
 */
void Token::parseInput(const CursesIO& io)
{
    for(char c; io >> c;)
    {
        io.printc(c);
    }
}
