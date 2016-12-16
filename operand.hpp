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
	protected:
		virtual void reduce() = 0;
	public:
		virtual void dbg(const string &s) = 0;
		virtual void compute(const ValToken *val) = 0;
		virtual void compute(const AddSubToken *val)  = 0;
		virtual void compute(const MulDivToken *val)  = 0;
		virtual void compute(const BracketToken *val) = 0;
};

/* TODO: Setup promoters, served and degraders operators */
class Expression : public Operand {
	private:
		TokenStack &stack;
		TokenStream &ts;
		CursesIO &io;
		unique_ptr<Token> currToken;
		bool shouldReturn = false;

		void reduce() override;

	public:
		Expression() = delete;
		Expression(TokenStack &filo, TokenStream &stream, CursesIO &io);

		void accept(Token *t);
		void run();
		void dbg(const string &s) override;
		void compute(const ValToken *val) override;
		void compute(const AddSubToken *val)  override;
		void compute(const MulDivToken *val)  override;
		void compute(const BracketToken *val) override;
};
