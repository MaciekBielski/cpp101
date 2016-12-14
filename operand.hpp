#pragma once

#include <memory>
#include <stack>

#include "curses_io.hpp"

class TokenStream;
class Token;
class ValToken;
class AddSubToken;
class MulDivToken;
class BracketToken;

using namespace std;
using TokenStack = stack<unique_ptr<Token>>;

class Operand {
	public:
		virtual void dbg(const string &s) = 0;
		virtual void compute(ValToken *val) = 0;
		virtual void compute(AddSubToken *val)  = 0;
		virtual void compute(MulDivToken *val)  = 0;
		virtual void compute(BracketToken *val) = 0;
};

/* TODO: Setup promoters, served and degraders operators */
class Expression : public Operand {
	private:
		TokenStack &stack;
		TokenStream &ts;
		CursesIO &io;
		unique_ptr<Token> currToken;
	public:
		Expression() = delete;
		Expression(TokenStack &filo, TokenStream &stream, CursesIO &io);

		void accept(Token *t);
		void run();
		void dbg(const string &s) override;
		void compute(ValToken *val) override;
		void compute(AddSubToken *val)  override;
		void compute(MulDivToken *val)  override;
		void compute(BracketToken *val) override;
};
