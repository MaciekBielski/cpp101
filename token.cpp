#include "token.hpp"

Token::Token(const string& r):
	raw{r}
{}

/* Becasue of proper filtering string should always be valid double */
ValToken::ValToken(const string& r):
	Token{r}, value{stod(r)}
{}

ValToken::ValToken(const double d):
	Token{to_string(d)}, value{d}
{}

ValToken::operator string() const
{
	return to_string(value);
}

double ValToken::getVal() const
{
	return value;
}

OpToken::OpToken(const string& r):
	Token{r}
{}

OpToken::operator string() const
{
	return raw;
}


AddSubToken::AddSubToken(const string &r):
	OpToken{r}
{}

MulDivToken::MulDivToken(const string &r):
	OpToken{r}
{}

BracketToken::BracketToken(const string &r):
	OpToken{r}
{}

FinToken::FinToken(const string &r):
	OpToken{r}
{}
//TODO: make FinToken
