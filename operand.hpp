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
class FinToken;

using namespace std;
using TokenStack = stack<unique_ptr<Token>>;

class Operand {
	protected:
		TokenStack &stack;
		TokenStream &ts;
		CursesIO &io;
		unique_ptr<Token> currToken;

		Operand(TokenStack &filo, TokenStream &stream, CursesIO &cio);
		virtual void reduce() = 0;

	public:
		TokenStack& getStack() const { return stack; }
		TokenStream& getStream() const { return ts; }
		CursesIO& getIO() const { return io; }
		void compute(const ValToken *val);

		virtual void dispatch(Token *t) = 0;
		virtual void run() = 0;
		virtual void dbg(const string &s) = 0;
		virtual void compute(const AddSubToken *val)  = 0;
		virtual void compute(const MulDivToken *val)  = 0;
		virtual void compute(const BracketToken *val) = 0;
		virtual void compute(const FinToken *val) = 0;
};

/* TODO: Setup promoters, served and degraders operators */
class Expression : public Operand {
	private:
		bool shouldReturn = false;

		void reduce() override;

	public:
		Expression() = delete;
		Expression(TokenStack &filo, TokenStream &stream, CursesIO &io);

		void dispatch(Token *t) override;
		void run() override;
		void dbg(const string &s) override;
		//void compute(const ValToken *val) override;
		void compute(const AddSubToken *val)  override;
		void compute(const MulDivToken *val)  override;
		void compute(const BracketToken *val) override;
		void compute(const FinToken *val) override;
};

class Term : public Operand {
	private:
		bool shouldReturn = false;

		void reduce() override;

	public:
		Term() = delete;
		Term(const Expression &exp);

		void dispatch(Token *t) override;
		void run() override;
		void dbg(const string &s) override;
		//void compute(const ValToken *val) override;
		void compute(const AddSubToken *val) override;
		void compute(const MulDivToken *val) override;
		void compute(const BracketToken *val) override;
		void compute(const FinToken *val) override;
};

