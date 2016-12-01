#include "token.hpp"

Token::Token(const string& r):
    raw{r}
{}

//TODO: interpret str as double
ValToken::ValToken(const string& r):
    Token(r)
{}

//TODO: interpret last char of a str
OpToken::OpToken(const string& r):
    Token(r)
{}
