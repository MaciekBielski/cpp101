#include "token_stream.hpp"

/*
 * CursesIO::operator>> performs already basic filtering so only characters
 * from predefined charset are returned at this level.
 *
 * Second-level filtering is that values are built from multiple characters and
 * operators are single-character. Also operators sometimes can invalidate
 * previous operators, assuming user made a mistake. Numbers have only care
 * about doubled period.
 */

/*
 * Takes stringstream's last character to decide the type of a token.
 * TODO: Return unique_ptr to the Token.
 */
static Token emitToken(const CursesIO &io, stringstream &acc)
{
    //auto toPass = Token{acc.str()};
    auto c = static_cast<char>(acc.seekg(-1, ios::basic_ios::end).peek());
    acc.seekg(0, ios::basic_ios::end);

    auto tokenFactory = [c](auto &chSet, auto &str) const {
        switch(c) {
            case chSet.isVal(c):
                return ValToken(str);
            case chSet.isAddOrSub(c):
            case chSet.isMulOrDiv(c):
            case chSet.isBracket(c):
                return OpToken(str);
        };
    };

    auto outToken = tokenFactory(io.getCharSet(), acc.str());

    io.err("Emit: "s + acc.str() + "last char:"s + c);
    acc.str("");
    acc.clear();

    return outToken;
}

static bool valAfterCloseBracket(stringstream& opAcc)
{
    bool out = false;

    opAcc.seekg(-1, ios::basic_ios::end);
    if (opAcc.peek() == ')')
        out = true;
    opAcc.seekg(0, ios::basic_ios::end);

    return out;
}

/* TODO: Does not always emit. Value should be returned by optional pointer
 */
static void firstCharOfVal(const char first, bool &hasDot, stringstream &inAcc,
        const CursesIO &io, stringstream *toEmit = nullptr)
{
    /* we are sure here that 'first' isVal, ignore it if after ')' */
    if (toEmit != nullptr && valAfterCloseBracket(*toEmit) )
    {
        io.err("Ignored "s + first + " after )");
        return;
    }

    if (toEmit != nullptr)
         emitToken(io, *toEmit);

    if (first == '.')
    {
        hasDot = true;
        io.acceptChar('0', inAcc);
    }
    io.acceptChar( first, inAcc);
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

/* updates both stream and screen*/
static void eraseIfTrailingDot(const CursesIO &io, stringstream &valAcc)
{
    valAcc.seekg(-1, ios::basic_ios::end);
    if (valAcc.peek() == '.')
    {
        auto s = valAcc.str();
        s.pop_back();
        valAcc.str(s);
        io.correctLast();                      // output correction
        io.err("Trailing dot removed");
    }
    valAcc.seekg(0, ios::basic_ios::end);
}

/* '(' has been already checked for */
static Token firstOpAfterVal(const char c, bool &hasDot, stringstream &acc,
        const CursesIO &io, uint& openBs, stringstream *toEmit = nullptr)
{
    /* if ')', then check if bracket allowed */
    if (c==')')
    {
        if (openBs)
            --openBs;
        else
        {
            io.err("Bracket not allowed");
            return;
        }
    }
    if (toEmit != nullptr)
    {
        eraseIfTrailingDot(io, *toEmit);
        emitToken(io, *toEmit);
    }
    hasDot = false;
    io.acceptChar( c, acc);
}

/* should acc become part of CursesIO? */
static inline const char getPrevChar(stringstream &acc)
{
    acc.seekg(-1, ios::basic_ios::end);
    return acc.peek();
}

static inline void correctPrevChar(char c, stringstream &acc)
{
    acc.seekp(-1, ios::basic_ios::end);
    acc << c;
}

/* op is emitted only when first number after op will be written, so
 * checks and corrections can be done here with screen updating.
 * Brackets control has to be done inside, not before or after.
 */
//XXX: here, the 'c' type should be statically setup to be 'Op', instead of char
//XXX: differentiate opAcc vs valAcc at least by name
static void opAfterOp(const char c, stringstream &acc, const CursesIO &io,
        unsigned int &openBrackets)
{
    /* [+] If prev was '/+-*' - accept '(', correct if '/+-*', ignore ')' */
    /* [+] If prev was '(' - accept '(', ignore '/+-*)' */
    /* [+] If prev was ')' - accept ')/+-*',  ignore '(' */
    /* test it! */
    /* another bracket control inside */
    const CharSet& chSet = io.getCharSet();

    switch(getPrevChar(acc))
    {
        case '/':
        case '*':
        case '+':
        case '-':
            if( chSet.isMulOrDiv(c) || chSet.isAddOrSub(c) )
            {
                correctPrevChar(c, acc);
                io.correctLast(&c);
                io.err("opAfterOp: CORRECT");
            }
            else if( c == '(' )
            {
                ++openBrackets;
                emitToken(io, acc);
                io.acceptChar(c, acc);
            }
            break;
        case '(':
            if( c == '(' )
            {
                ++openBrackets;
                emitToken(io, acc);
                io.acceptChar(c, acc);
            }
            break;
        case ')':
            /* '(' ignored */
            if( chSet.isMulOrDiv(c) || chSet.isAddOrSub(c)
                    || ( c==')' && openBrackets>0 ) )
            {
                if( c==')' )
                    --openBrackets;
                emitToken(io, acc);
                io.acceptChar(c, acc);
            }
            break;
    }

}

/*
 * This should filter whatever cannot be printed on screen during typing
 * Accummulators are keeping what will be emited as a token but have nothing in
 * common with the screen
 *
 * - firstCharOfVal
 * - firstOpAfterVal
 *   - createToken must do same cleanup as emitToken now
 */
void TokenStream::parseInput(const CursesIO &io) const
{
    static auto valAcc = stringstream{};
    static auto opAcc = stringstream{};
    auto hasDot = false, getFirst = true;
    auto openBrackets = 0u;
    auto chSet = io.getCharSet();

    /* First charcter of input: ")*+/" are ignored at the beginning */ 
    char first;
    do {
        io >> first;
        if( chSet.isVal(first) )
        {
            firstCharOfVal(first, hasDot, valAcc, io);
            getFirst = false;
        }
        else if ( first == '(' )
        {
            io.acceptChar(first, opAcc);
            ++openBrackets;
            getFirst = false;
        }
    } while(getFirst);
    io.err("First valid character taken");

    /* 2nd and further characters */
    for(char c; io >> c;)
    {
        if( chSet.isVal(c) )
            opAcc.str().empty() ?
                notFirstCharOfVal(c, hasDot, valAcc, io) :
                firstCharOfVal(c, hasDot, valAcc, io, &opAcc);
        else
        {
            /* previous character was a digit - '(' is ignored */
            if( !valAcc.str().empty() && c != '(' )
                firstOpAfterVal(c, hasDot, opAcc, io, openBrackets, &valAcc);
            /* operator following another operator, */
            else if( !opAcc.str().empty() > 0 )
                opAfterOp(c, opAcc, io, openBrackets);
        }
    }
}
