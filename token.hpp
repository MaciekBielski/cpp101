#pragma once

#include <string>

#include "operand.hpp"

/* trampoline to pass a correct 'this' to visiting Operand */
#define _compute(Arg) \
	void compute(Arg &arg) override {	\
			arg.compute(this); \
	}

using namespace std;

class Token {
	protected:
		const string raw;
		Token(const string& r);
	public:
		virtual ~Token() {};

		virtual operator string() const = 0;
		virtual void compute(Operand &opd) = 0;
};



class ValToken final : public Token {
	private:
		double value = 0;
	public:
		ValToken(const string& r);
		~ValToken(){};

		operator string() const override;
		_compute(Operand);
};

class OpToken : public Token {
	protected:
		OpToken(const string &r);
	public:
		~OpToken(){};

		virtual operator string() const override;
};

class AddSubToken : public OpToken {
	public:
		AddSubToken(const string &r);
		_compute(Operand);
};

class MulDivToken : public OpToken {
	public:
		MulDivToken(const string &r);
		_compute(Operand);
};

class BracketToken : public OpToken {
	public:
		BracketToken(const string &r);
		_compute(Operand);
};
