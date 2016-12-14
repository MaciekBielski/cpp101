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
		io.err("Passed: "s + static_cast<string>(*currToken));
		/* get underlying token */
		accept(currToken.get());
	} while (!done);
}

void Expression::dbg(const string &s)
{
	io.err(s);
}

void Expression::compute(ValToken *val)
{
	dbg("ValToken"s);
}

void Expression::compute(AddSubToken *val)
{
	dbg("AddSubToken"s);
}

void Expression::compute(MulDivToken *val)
{
	dbg("MulDivToken"s);
}

void Expression::compute(BracketToken *val)
{
	dbg("BracketToken"s);
}
