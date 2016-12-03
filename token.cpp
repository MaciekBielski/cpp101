#include "token.hpp"

Token::Token(const string& r):
    raw{r}
{}

/* Becasue of proper filtering string should always be valid double */
ValToken::ValToken(const string& r):
    Token(r), value(stod(r)) 
{}

ValToken::operator string()
{
    return "Value: "s + to_string(value);
}
//TODO: interpret last char of a str
OpToken::OpToken(const string& r):
    Token(r)
{}

OpToken::operator string()
{
    return "Operator: "s + raw;
}
