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
static void emit(const CursesIO &io, stringstream &acc)
{
    //TODO: check for stupid corrections of a last sign
    io.err(acc.str());
    acc.str("");
    acc.clear();
}

static void firstCharOfVal(const char first, bool &hasDot, stringstream &acc,
        const CursesIO &io)
{
    if(first == '.')
    {
        acc << '0';
        hasDot = true;
        io.printc(0);
    }
    acc << first;
    io.printc(first);
}

static void notFirstCharOfVal(const char c, bool &hasDot,
        stringstream &acc, const CursesIO &io)
{
    if( c=='.' && hasDot)
        return;
    if( c=='.' && !hasDot)
        hasDot = true;
    acc << c;
    io.printc(c);
}

/* '(' has been already checked for */
static void firstOpAfterVal(const char c, bool &hasDot,
        stringstream &acc, const CursesIO &io)
{
    hasDot = false;
    acc << c
    io.printc(c);
}

/* checks whether previous operator should be emitted or corrected/ignored */
static void opAfterOp(const char c, stringstream &acc, const CursesIO &io)
{
//TODO: finished here
   /* ')' corrects previous op, unless it was also ')' */ 
   /* '(' only after '+/-*' */ 
   /* '+/-*' correct previous one, unless it was ')' */ 
}

/* This should filter whatever cannot be printed on screen during typing */
void Token::parseInput(const CursesIO &io)
{
    static stringstream valAcc {};
    static stringstream opAcc {};
    const CharSet& chSet = io.getCharSet();
    bool hasDot = false;

    /* First charcter of input: ")*+/" are ignored at the beginning */ 
    char first;
    io >> first;
    if( chSet.isVal(first) )
    {
        firstCharOfVal(first, hasDot, valAcc, io);
    }
    else if ( first == '(' )
    {
        opAcc << first;
        io.printc(first);
    }

    /* 2nd and furthers characters */
    for(char c; io >> c;)
    {
        assert( valAcc.rdbuf()->in_avail() >= 0 && valAcc.rdbuf()->in_avail() >= 0 );
        if( chSet.isVal(c) )
        {
            /* first character for this value */
            if( opAcc.rdbuf()->in_avail() > 0 )
            {
                emit(io, opAcc);
                /* again, .234 -> 0.234 */
                firstCharOfVal(c, hasDot, valAcc, io);
            }
            else /* non-first char for this value */
            {
                notFirstCharOfVal(c, hasDot, valAcc, io);
                //TODO: what about emit
            }
        }
        else
        {
            /* previous character was a digit - '(' is ignored */
            if( valAcc.rdbuf()->in_avail() > 0 && c != '(' )
            {
                emit(io, valAcc);
                firstOpAfterVal(c, hasDot, opAcc, io);
            }
            /*
             * operator following another operator, */
            else if( opAcc.rdbuf()->in_avail() > 0 )
            {
                opAfterOp(c, opAcc, io);
                //emit(io, valAcc);
            }
        }
        io.printc(c);
    }
}
