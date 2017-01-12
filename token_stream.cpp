#include "token_stream.hpp"

#include <cassert>
#include <memory>
#include <utility>
#include <thread>

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
 * end of the program and at the bottom of parseInput loop the token is
 * produced ad-hoc
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

/* useful to check because that is incorrect */
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

/*
 * This function is never called for '(' since '123(' is incorrect. ')' is
 * allowed only if there is already '(' to pair with. '=' is allowed only when
 * all brackets are closed and is not printed on the screen. Building token
 * from '=' is done ad-hoc in the parseInput loop, not here.
 */
static void firstOpAfterVal(const char c, bool &hasDot, stringstream &acc,
		const CursesIO &io, uint& openBs, unique_ptr<Token> &token,
		stringstream *toEmit = nullptr)
{
	/* if ')', then check if bracket allowed */
	if (c==')') {
		if (openBs>0) {
			--openBs;
			io.err("openBs: "s + to_string(openBs));
		}
		else {
			io.err("No bracket is open, ignored!"s);
			return;
		}
	}
	/* if '=' then allow only if all brackets paired, otherwise return */
	else if (io.getCharSet().isFin(c) && !!openBs) {
		io.err("Brackets not balanced\n"s);
		return;
	}
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

/* Op is emitted only when first number after op will be written, so when
 * operator comes after operator usually the previous one is overwritten and
 * noting is emitted, with some exceptions.
 * More specific rules roughly:
 * - prev was '/+-*' - accept '(', correct if '/+-*=', ignore ')'
 * - prev was '(' - accept '(', ignore '/+-*)='
 * - prev was ')' - accept ')/+-*=',  ignore '('
 * - '=' emits the token immediately
 */
static void opAfterOp(const char c, stringstream &acc, const CursesIO &io,
		unsigned int &openBs, unique_ptr<Token> &emited)
{
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
			} else if (chSet.isFin(c) && !openBs) {
				correctPrevChar(c, acc);
				io.correctLast(nullptr);
				io.err("SHOULD RETURN"s);
				//TODO: token will be emitted immediately
			} else if (c == '(') {
				++openBs;
				io.err("openBs: "s + to_string(openBs));
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			}
			break;
		case '(':
			if ( c == '(' ) {
				++openBs;
				io.err("openBs: "s + to_string(openBs));
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			}
			break;
		case ')':
			if (chSet.isMulOrDiv(c) || chSet.isAddOrSub(c)) {
				emited = emitToken(io, acc);
				io.acceptChar(c, acc);
			} else if (c==')') {
				if (openBs) {
					--openBs;
					io.err("openBs: "s + to_string(openBs));
					emited = emitToken(io, acc);
					io.acceptChar(c, acc);
				} else {
					io.err("Brackets not balanced\n"s);
				}
			} else if (chSet.isFin(c)) {
				if (!openBs) {
					io.err("SHOULD RETURN"s);
					emited = emitToken(io, acc);
					io.acceptChar(c, acc);
					io.correctLast(nullptr);
				} else {
					io.err("Brackets not balanced\n"s);
				}
			}
			break;
	}

}

/*
 * This function is Tokens producer.
 *
 * This should filter whatever cannot be printed on screen during typing.
 * Accummulators `valAcc` and `opAcc` are collecting characters that will be
 * emited as a token afterwards. `opAcc` is always made of one character but
 * `valAcc` can include multiple digits and one dot.
 *
 * Token is created when the first character of a new token will come, eg '3'
 * after '+' or '*' after '8'. Emission takes place as follows:
 * the loop that collects characters wait until token is null, then iterates
 * until all characters of a token are collected. After that `token` becomes
 * not null and this iteration performs additional step, notifying the main
 * thread that receives produced tokens. The next iteration waits again until
 * the token will become consumed.
 *
 * There is a corner-case of '=', when it has to be emitted in the same
 * iteration since no further characters is expected to be received.
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
			bufLk.unlock();
			bufCv.notify_one();
			// yield here

			if (chSet.isFin(c)) {
				//this cannot be synchronised with the same lock because
				//it can get locked immediately before `passToken` will consume
				//`token`, but because it happens only once at the end it is
				//simpler to make waiting loop until `token` is null.
				bufLk.lock();
				while (token) {
					bufLk.unlock();
					this_thread::sleep_for(100ms);
					bufLk.lock();
				}
				io.err("Emit last token"s);
				
				token.reset(new FinToken(opAcc.str()));
				bufLk.unlock();
				bufCv.notify_one();
				break;
			}
		}
	}
}

/*
 * Tokens consumer, called by Expression and Term.
 *
 * First checks if previously obtained token has been pushed back as a
 * `lastToken`. This is used when Term receives tokens that are not handled by
 * Term, then such token is pushed back and Term returns to its caller
 * (Expression), which deals with it.
 */
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
		io.err(__func__ + "Get new: "s + static_cast<string>(*out));

		assert(!token);
		// let the parseInput run another iteration
		bufLock.unlock();
	}

	return out;
}
