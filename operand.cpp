#include "operand.hpp"

#include "token.hpp"
#include "token_stream.hpp"

Expression::Expression(TokenStack &filo, TokenStream &stream, CursesIO &cio):
	stack{filo}, ts{stream}, io{cio}
{}

/* TODO: Thing about visitor here how to make it short */
void Expression::accept(Token *t)
{
	t->compute(*this);
}

void Expression::run()
{
	stack.push(make_unique<ValToken>("0"s));
	stack.push(make_unique<AddSubToken>("+"s));

	auto done = bool {false};

	do {
		currToken = ts.passToken(io);
		dbg("Passed: "s + static_cast<string>(*currToken));
		/* get underlying token */
		accept(currToken.get());
	} while (!done);
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
	dbg("AddSubToken"s);
}

void Expression::compute(const MulDivToken *val)
{
	/* push the operator and runTerm */
	dbg("MulDivToken"s);
}

void Expression::compute(const BracketToken *val)
{
	/* runTerm if '(' */
	/* reduce, discard the operator and return if ')' or '=' */
	dbg("BracketToken"s);
}

//TODO: reduce function
