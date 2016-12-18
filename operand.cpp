#include <cassert>

#include "operand.hpp"

#include "token.hpp"
#include "token_stream.hpp"

#define _dispatchMeth(Klass) \
	void Klass::dispatch(Token *t) {	\
			t->compute(*this); \
	}

Operand::Operand(TokenStack &filo, TokenStream &stream, CursesIO &cio):
	stack{filo}, ts{stream}, io{cio}
{}

void Operand::compute(const ValToken *val)
{
	/* push the value on stack and return */
	stack.push(move(currToken));
	//dbg("Pushed "s + static_cast<string>(*val));
}

Expression::Expression(TokenStack &filo, TokenStream &stream, CursesIO &cio):
	Operand{filo, stream, cio}
{}

void Expression::reduce()
{
	auto rTok = static_cast<ValToken*> (stack.top().release());
	stack.pop();
	auto opTokStr = static_cast<string> (*(stack.top().release()));
	stack.pop();
	auto lTok = static_cast<ValToken*> (stack.top().release());
	stack.pop();

	assert( ("+"s == opTokStr) || ("-"s == opTokStr));
	auto resVal = ("+"s == opTokStr) ?
		lTok->getVal() + rTok->getVal() :
		lTok->getVal() - rTok->getVal();

	auto resToken = make_unique<ValToken> (resVal); 

	dbg("rVal: "s + static_cast<string>(*rTok) +
		" lVal: "s + static_cast<string>(*lTok) +
		" op: "s + opTokStr +
		" pushed: "s + static_cast<string>(*resToken));

	stack.push(move(resToken));
}

_dispatchMeth(Expression);

void Expression::run()
{
	stack.push(make_unique<ValToken>("0"s));
	stack.push(make_unique<AddSubToken>("+"s));

	do {
		currToken = ts.passToken(io);
		/* get underlying token */
		dispatch(currToken.get());
	} while (!shouldReturn);
}

inline void Expression::dbg(const string &s) { io.err(s); }

/* reduce and push the operator */
void Expression::compute(const AddSubToken *val)
{
	reduce();
	stack.push(move(currToken));
}

/* push the operator and runTerm */
void Expression::compute(const MulDivToken *val)
{
	stack.push(move(currToken));
	auto term = Term{*this};
	dbg("Expression run new Term"s);
	term.run();
}

/* run new Expression if '(', otherwise reduce and quit this expression */
void Expression::compute(const BracketToken *val)
{
	auto opTokStr = static_cast<string> (*val);
	currToken.release();			//this can be forgotten

	if ("("s == opTokStr) {
		auto exp = Expression{stack, ts, io};
		dbg("Expression run new expression"s);
		exp.run();
	} else {
		reduce();
		dbg("Expression return"s);
		shouldReturn = true;
	}
}

////////////////////////////////////////////////////////////////////////////
Term::Term(const Expression &exp):
	Operand{exp.getStack(), exp.getStream(), exp.getIO()}
{
	dbg("Term created");
}

void Term::reduce()
{
	auto rTok = static_cast<ValToken*> (stack.top().release());
	stack.pop();
	auto opTokStr = static_cast<string> (*(stack.top().release()));
	stack.pop();
	auto lTok = static_cast<ValToken*> (stack.top().release());
	stack.pop();

	assert( ("*"s == opTokStr) || ("/"s == opTokStr));
	auto resVal = ("*"s == opTokStr) ?
		lTok->getVal() * rTok->getVal() :
		lTok->getVal() / rTok->getVal();

	auto resToken = make_unique<ValToken> (resVal); 

	dbg("rVal: "s + static_cast<string>(*rTok) +
		" lVal: "s + static_cast<string>(*lTok) +
		" op: "s + opTokStr +
		" result: "s + static_cast<string>(*resToken));

	stack.push(move(resToken));
}

_dispatchMeth(Term)

void Term::run()
{
	do {
		currToken = ts.passToken(io);
		/* get underlying token */
		dispatch(currToken.get());
	} while (!shouldReturn);
}

inline void Term::dbg(const string &s) { io.err(s); }

/* reduce, push back the token to be fetched at Expression level*/
void Term::compute(const AddSubToken *val)
{
	reduce();
	dbg("Term pushBack "s + static_cast<string>(*val));
	ts.pushBackToken(currToken);
	shouldReturn = true;
}

void Term::compute(const MulDivToken *val)
{
	reduce();
	dbg("Term pushBack "s + static_cast<string>(*val));
	stack.push(move(currToken));
}

/* run new Exp if (, reduce, pushBack and return if ) or = */
void Term::compute(const BracketToken *val)
{
	auto opTokStr = static_cast<string> (*val);

	if ("("s == opTokStr) {
		auto exp = Expression{stack, ts, io};
		dbg("Running new expression"s);
		currToken.release();
		exp.run();
	} else {
		reduce();
		dbg("Term pushBack "s + static_cast<string>(*val));
		ts.pushBackToken(currToken);
		shouldReturn = true;
	}
}
