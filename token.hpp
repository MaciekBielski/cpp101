#pragma once

#include <string>

using namespace std;

/*
 * This design needs to be rethought.
 * TODO: all should have move constructors.
 */
class Token {
    protected:
        const string raw;
        Token(const string& r);
    public:
        virtual ~Token() {};

        virtual operator string() = 0;
};

class ValToken final : public Token {
    private:
        double value = 0;
    public:
        ValToken(const string& r);
        ~ValToken(){};

        operator string() override;
};

class OpToken : public Token {
    public:
        OpToken(const string& r);
        ~OpToken(){};

        virtual operator string() override;
};
