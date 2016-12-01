#pragma once

#include <string>

using namespace std;

/*
 * This design needs to be rethought.
 * TODO: all should have move constructors.
 */
class Token {
    private:
        const string raw;
    protected:
        Token(const string& r);
};

class ValToken : public Token {
    private:
        double value = 0;
    public:
        ValToken(const string& r);
};

class OpToken : public Token {
    public:
        OpToken(const string& r);
};
