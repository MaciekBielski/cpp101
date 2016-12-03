#include "token_stream.hpp"

#include <cassert>
#include <memory>
#include <utility>

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
static unique_ptr<Token> emitToken(const CursesIO &io, stringstream &acc)
{
    //auto toPass = Token{acc.str()};
    auto c = static_cast<char>(acc.seekg(-1, ios::basic_ios::end).peek());
    acc.seekg(0, ios::basic_ios::end);

    auto tokenFactory = [c](auto &chSet, auto str) {
        unique_ptr<Token> out = nullptr;

        if (chSet.isVal(c))
            out = make_unique<ValToken>(str);
        else if ( chSet.isAddOrSub(c) ||
                chSet.isMulOrDiv(c) ||
                chSet.isBracket(c) )
            out = make_unique<OpToken>(str);

        return out;
    };

    auto outToken = tokenFactory(io.getCharSet(), acc.str());

    io.err("Last char:"s + c);
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
        const CursesIO &io, unique_ptr<Token> *emited = nullptr,
        stringstream *toEmit = nullptr)
{
    /* we are sure here that 'first' isVal, ignore it if after ')' */
    if (toEmit != nullptr && valAfterCloseBracket(*toEmit) )
    {
        io.err("Ignored "s + first + " after )");
        return;
    }

    if (toEmit != nullptr)
         *emited = emitToken(io, *toEmit);

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
static void firstOpAfterVal(const char c, bool &hasDot, stringstream &acc,
        const CursesIO &io, uint& openBs, unique_ptr<Token> &token,
        stringstream *toEmit = nullptr)
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
        token = emitToken(io, *toEmit);
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
        unsigned int &openBrackets, unique_ptr<Token> &emited)
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
                emited = emitToken(io, acc);
                io.acceptChar(c, acc);
            }
            break;
        case '(':
            if( c == '(' )
            {
                ++openBrackets;
                emited = emitToken(io, acc);
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
                emited = emitToken(io, acc);
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
 */
void TokenStream::parseInput(const CursesIO &io)
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
            //TODO: this function prototype is not so nice
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
        // sync: wait till token is null
        unique_lock<mutex> bufLk {bufMtx};
        bufCv.wait(bufLk, [this]{ return !(this->token); });

        if( chSet.isVal(c) )
            opAcc.str().empty() ?
                notFirstCharOfVal(c, hasDot, valAcc, io) :
                firstCharOfVal(c, hasDot, valAcc, io, &token, &opAcc);
        else
        {
            /* previous character was a digit - '(' is ignored */
            if( !valAcc.str().empty() && c != '(' )
                firstOpAfterVal(c, hasDot, opAcc, io, openBrackets, token, &valAcc);
            /* operator following another operator, */
            else if( !opAcc.str().empty() > 0 )
                opAfterOp(c, opAcc, io, openBrackets, token);
        }

        if(token)
        {
            //io.err("Emitted "s + static_cast<string>(*token));

            // sync: wait till token is null
            bufLk.unlock();
            bufCv.notify_one();
            // yield here
        }
    }
}

void TokenStream::passToken(const CursesIO &io)
{
    // sync: wait till token is not null
    unique_lock<mutex> bufLock {bufMtx};
    bufCv.wait(bufLock, [this]{ return !!(this->token); });

    // process the token and nullify it
    // test that after moving the token indeed is null
    auto out = std::move(token);

    io.err("Passed: "s + static_cast<string>(*out));
    assert(!token);
    // let the parseInput run another iteration
    bufLock.unlock();
}
