#pragma once

#include <string>

using namespace std;

/*
 * This design needs to be rethought.
 */
class Token {
    private:
        const string raw;
    public:
        Token(const string& r);
};

class Value : public Token {
    private:
        double value = 0;
    public:
        Val(const string& r);
};

class Operator : public Token {
    public:
        Operator(const string& r);
};
