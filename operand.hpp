#pragma once

#include <memory>
#include <stack>

#include "curses_io.hpp"

class TokenStream;
class Token;

using namespace std;
using TokenStack = stack<unique_ptr<Token>>;

/* TODO: Setup promoters, served and degraders operators */
class Expression final {
	private:
		TokenStack &stack;
		TokenStream &ts;
		CursesIO &io;
	public:
		Expression() = delete;
		Expression(TokenStack &filo, TokenStream &stream, CursesIO &io);

		void accept(unique_ptr<Token>);
		void run();
		void dbg(const string &s);
};
