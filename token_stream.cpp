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
 *
 * The case of '=' emitting is not handled here since it occurs only at the
 * end of the program and the unique_ptr::reset is used for that at the bottom
 * of parseInput loop
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
		else if (chSet.isAddOrSub(c))
			out = make_unique<AddSubToken>(str);
		else if (chSet.isMulOrDiv(c))
			out = make_unique<MulDivToken>(str);
		else if (chSet.isBracket(c))
			out = make_unique<BracketToken>(str);

		return out;
	};

	auto outToken = tokenFactory(io.getCharSet(), acc.str());

	//io.err("Last char:"s + c);
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

/*
 * - toEmit: keeps the previous operator, used to detect whether this is a
 *   character after operator or the very first character of an expression and
 *   to create a Token if allowed (read below)
 * - emited: used to pass the created Token
 *
 * When the very first character is parsed both `emited` and `toEmit` pointers
 * are null. Otherwise `first` is a character of the value that follows an
 * operator. In that case nothing is printed on the screen since the sequence
 * ')123' is incorrect.
 * If the value starts from `.`, `0` is appended automatically
 */
static void firstCharOfVal(const char first, bool &hasDot, stringstream &inAcc,
		const CursesIO &io, unique_ptr<Token> *emited = nullptr,
		stringstream *toEmit = nullptr)
{
	/* we are sure here that 'first' isVal, ignore it if after ')' */
	if (toEmit != nullptr && valAfterCloseBracket(*toEmit) )
		return;

	if (toEmit != nullptr)
		 *emited = emitToken(io, *toEmit);

	if (first == '.') {
		hasDot = true;
		io.acceptChar('0', inAcc);
	}
	io.acceptChar( first, inAcc);
}

/* 
 * Accumulates characters of the value, does not allow for more than one dot
 */
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
		io.correctLast();					  // output correction
		//io.err("Trailing dot removed");
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
		if (openBs)
			--openBs;
		else
			return;
	/* if '=' then allow only if all brackets paired, otherwise return */
	else if (io.getCharSet().isFin(c) && !!openBs)
		return;
	/* emit value */
	if (toEmit != nullptr) {
		eraseIfTrailingDot(io, *toEmit);
		token = emitToken(io, *toEmit);
	}
	hasDot = false;
	io.acceptChar(c, acc);
	//if '=' then correct last character on screen and emit immediately
	if (io.getCharSet().isFin(c) && !openBs) {
		io.correctLast(nullptr);
		io.err("SHOULD RETURN"s);
	}
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
		unsigned int &openBs, unique_ptr<Token> &emited)
{
	/* [+] If prev was '/+-*' - accept '(', correct if '/+-*=', ignore ')' */
	/* [+] If prev was '(' - accept '(', ignore '/+-*)=' */
	/* [+] If prev was ')' - accept ')/+-*=',  ignore '(' */
	/* '=' emits the token immediately */
	/* test it! */
	/* another bracket control inside */
	const CharSet& chSet = io.getCharSet();

	switch(getPrevChar(acc))
	{
		case '/':
		case '*':
		case '+':
		case '-':
			if (chSet.isMulOrDiv(c) || chSet.isAddOrSub(c)) {
				correctPrevChar(c, acc);
				io.correctLast(&c);
				//io.err("opAfterOp: CORRECT");
			} else if (chSet.isFin(c) && !openBs) {
				correctPrevChar(c, acc);
				io.correctLast(nullptr);
				io.err("SHOULD RETURN"s);
				//TODO: token will be emitted immediately
			} else if (c == '(') {
				++openBs;
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			}
			break;
		case '(':
			if ( c == '(' )
			{
				++openBs;
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			}
			break;
		case ')':
			/* '(' ignored */
			//TODO: implement '=' and test
			//TODO: test for brackets - not tested fix
			if (chSet.isMulOrDiv(c) || chSet.isAddOrSub(c) ||
					chSet.isBracket(c)) {
				if (c==')')
					--openBs;
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			} else if (chSet.isFin(c) && !openBs) {
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
				io.correctLast(nullptr);
				io.err("SHOULD RETURN"s);
			}
			break;
	}

}

/*
 * This should filter whatever cannot be printed on screen during typing
 * Accummulators `valAcc` and `opAcc` are keeping what will be emited as a
 * token after all symbols are collected. `opAcc` is always made of one
 * character but `valAcc` can include multiple digits and one dot.
 */
void TokenStream::parseInput(const CursesIO &io)
{
	static auto valAcc = stringstream{};
	static auto opAcc = stringstream{};
	auto hasDot = false, getFirst = true;
	auto openBrackets = 0u;
	auto chSet = io.getCharSet();

	/*
	 * First character of input: ")*+/=" make no sense at the beginning and are
	 * ignored
	 */ 
	char first;
	do {
		io >> first;
		if(chSet.isVal(first)) {
			firstCharOfVal(first, hasDot, valAcc, io);
			getFirst = false;
		}
		else if (first == '(') {
			io.acceptChar(first, opAcc);
			++openBrackets;
			getFirst = false;
		}
	} while (getFirst);

	/* 2nd and further characters */
	for (char c; io >> c;) {
		unique_lock<mutex> bufLk {bufMtx};
		bufCv.wait(bufLk, [this]{ return !(this->token); });

		if (chSet.isVal(c)) {
			opAcc.str().empty() ?
				notFirstCharOfVal(c, hasDot, valAcc, io) :
				firstCharOfVal(c, hasDot, valAcc, io, &token, &opAcc);
		} else {
			/* previous character was a digit - '(' is ignored */
			if (!valAcc.str().empty() && c != '(')
				firstOpAfterVal(c, hasDot, opAcc, io, openBrackets, token, &valAcc);
			/* operator following another operator, */
			else if (!opAcc.str().empty() > 0)
				opAfterOp(c, opAcc, io, openBrackets, token);
		}

		if (token) {
			//io.err("Emitted "s + static_cast<string>(*token));

			bufLk.unlock();
			bufCv.notify_one();
			// yield here

			//TODO: if last character was '=' then after emitting value emit
			//the token immediately
			if (chSet.isFin(c)) {
				io.err("Emitting RETURN immediately"s);
				bufLk.lock();
				bufCv.wait(bufLk, [this]{ return !(this->token); });
				
				token.reset(new FinToken(opAcc.str()));
				bufLk.unlock();
				bufCv.notify_one();
				break;
			}
		}
	}
}

unique_ptr<Token> TokenStream::passToken(const CursesIO &io)
{
	auto out = unique_ptr<Token>{nullptr};

	if (lastToken) {
		io.err("Get cached: "s + static_cast<string>(*lastToken));
		out = std::move(lastToken);
		assert(!lastToken);
	} else {
		// sync: wait till token is not null
		unique_lock<mutex> bufLock {bufMtx};
		bufCv.wait(bufLock, [this]{ return !!(this->token); });

		// move the ownership
		out = std::move(token);
		io.err("Get new: "s + static_cast<string>(*out));

		assert(!token);
		// let the parseInput run another iteration
		bufLock.unlock();
	}

	return out;
}
