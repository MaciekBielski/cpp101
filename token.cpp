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

// XXX: this should be made class method
static void emit(const CursesIO &io, stringstream &acc)
{
    //TODO: check for stupid corrections of a last sign - this is interpreting
    //part?
    io.err(acc.str());
    acc.str("");
    acc.clear();
}

static void firstCharOfVal(const char first, bool &hasDot, stringstream &acc,
        const CursesIO &io)
{
    if(first == '.')
    {
        hasDot = true;
        io.acceptChar('0', acc);
    }
    io.acceptChar( first, acc);
}

static void notFirstCharOfVal(const char c, bool &hasDot,
        stringstream &acc, const CursesIO &io)
{
    if( c=='.' && hasDot)
        return;
    if( c=='.' && !hasDot)
        hasDot = true;
    io.acceptChar( c, acc);
}

/* '(' has been already checked for */
static void firstOpAfterVal(const char c, bool &hasDot,
        stringstream &acc, const CursesIO &io)
{
    hasDot = false;
    io.acceptChar( c, acc);
}

/* should acc become part of CursesIO? */
static inline const char getPrevChar(stringstream &acc)
{
    acc.seekg(-1, ios::basic_ios::end);
    return acc.peek();
}

/* should acc become part of CursesIO? */
static inline void correctPrevChar(char c, stringstream &acc, const CursesIO &io)
{
    acc.seekp(-1, ios::basic_ios::end);
    acc << c;
    io.correctLast(c);
}

/* op is emitted only when first number after op will be written, so
 * checks and corrections can be done here with screen updating */
//XXX: here, the 'c' type should be statically setup to be 'Op', instead of char
//XXX: differentiate opAcc vs valAcc at least by name
static void opAfterOp(const char c, stringstream &acc, const CursesIO &io)
{
    /* [+] If prev was '/+-*' - accept '(', correct if '/+-*', ignore ')' */
    /* [+] If prev was '(' - accept '(', ignore '/+-*)' */
    /* [+] If prev was ')' - accept ')/+-*',  ignore '(' */
    /* test it! */
    const CharSet& chSet = io.getCharSet();

    switch(getPrevChar(acc))
    {
        case '/':
        case '*':
        case '+':
        case '-':
            if( chSet.isMulOrDiv(c) || chSet.isAddOrSub(c) )
                correctPrevChar(c, acc, io);
            else if( c == '(' )
            {
                //XXX: acc =? opAcc
                emit(io, acc);
                io.acceptChar(c, acc);
            }
            break;
        case '(':
            if( c == '(' )
            {
                emit(io, acc);
                io.acceptChar(c, acc);
            }
            break;
        case ')':
            if( chSet.isMulOrDiv(c) || chSet.isAddOrSub(c) || c==')' )
            {
                emit(io, acc);
                io.acceptChar(c, acc);
            }
            break;
    }

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

    /* 2nd and further characters */
    for(char c; io >> c;)
    {
        if( chSet.isVal(c) )
        {
            /* first character for this value */
            if( opAcc.rdbuf()->in_avail() > 0 )
            {
                // XXX: emit should be invoked from firstCharOfVal after it
                // will become method
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
                // XXX: emit should be invoked from firstOpAfterVal after it
                // will become method
                emit(io, valAcc);
                firstOpAfterVal(c, hasDot, opAcc, io);
            }
            /*
             * operator following another operator, */
            else if( opAcc.rdbuf()->in_avail() > 0 )
            {
                opAfterOp(c, opAcc, io);
            }
        }
        io.printc(c);
    }
}
