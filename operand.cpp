#include "operand.hpp"

#include "token.hpp"
#include "token_stream.hpp"

Expression::Expression(TokenStack &filo, TokenStream &stream, CursesIO &cio):
	stack{filo}, ts{stream}, io{cio}
{}

void Expression::reduce()
{
	auto rTok = static_cast<ValToken*> (move(stack.top()).release());
	stack.pop();
	auto opTok = static_cast<AddSubToken*> (move(stack.top()).release());
	stack.pop();
	auto lTok = static_cast<ValToken*> (move(stack.top()).release());
	stack.pop();

	auto resVal = ("+"s == static_cast<string> (*opTok)) ?
		lTok->getVal() + rTok->getVal() :
		lTok->getVal() - rTok->getVal();

	auto resToken = make_unique<ValToken> (resVal); 

	dbg("rVal: "s + static_cast<string>(*rTok) +
		" lVal: "s + static_cast<string>(*lTok) +
		" op: "s + static_cast<string>(*opTok) +
		" pushed: "s + static_cast<string>(*resToken));

	stack.push(move(resToken));
}

/* TODO: Thing about visitor here how to make it short */
void Expression::accept(Token *t)
{
	t->compute(*this);
}

void Expression::run()
{
	stack.push(make_unique<ValToken>("0"s));
	stack.push(make_unique<AddSubToken>("+"s));

	do {
		currToken = ts.passToken(io);
		dbg("Passed: "s + static_cast<string>(*currToken));
		/* get underlying token */
		accept(currToken.get());
	} while (!shouldReturn);
}

inline void Expression::dbg(const string &s) { io.err(s); }

/* Pointers were only for visitor dispatching, real token handler is currToken */
void Expression::compute(const ValToken *val)
{
	/* push the value on stack and return */
	stack.push(move(currToken));
	dbg("Pushed "s + static_cast<string>(*val));
}

void Expression::compute(const AddSubToken *val)
{
	/* reduce and push the operator */
	reduce();
	stack.push(move(currToken));
	dbg("AddSubToken"s);
}

void Expression::compute(const MulDivToken *val)
{
	/* push the operator and runTerm */
	stack.push(move(currToken));
	//TODO: runTerm
	dbg("MulDivToken"s);
}

void Expression::compute(const BracketToken *val)
{
	/* runTerm if '(' */
	/* reduce, discard the operator and return if ')' or '=' */
	dbg("BracketToken"s);
	reduce();
	currToken.release();
	if ("="s == static_cast<string>(*val))
		shouldReturn = true;
}
