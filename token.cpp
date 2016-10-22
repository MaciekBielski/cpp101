#include "token.hpp"

void Token::parseInput( const CursesIO& io)
{
    for(char c; io >> c;)
        io.printc(c);
}
