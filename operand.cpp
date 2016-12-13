#include "operand.hpp"

#include "token.hpp"
#include "token_stream.hpp"

Expression::Expression(TokenStack &filo, TokenStream &stream, CursesIO &cio):
	stack{filo}, ts{stream}, io{cio}
{}

/* TODO: Thing about visitor here how to make it short */
void Expression::accept(unique_ptr<Token> t)
{
	t->compute(*this);
}

void Expression::run()
{
	stack.push(make_unique<ValToken>("0"s));
	stack.push(make_unique<AddSubToken>("+"s));

	auto done = bool {false};

	do {
		auto tokenPtr = ts.passToken(io);
		io.err("Passed: "s + static_cast<string>(*tokenPtr));
		accept(move(tokenPtr));
	} while (!done);
}

void Expression::dbg(const string &s)
{
	io.err(s);
}
