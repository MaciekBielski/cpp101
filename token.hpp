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
    protected:
        OpToken(const string &r);
    public:
        ~OpToken(){};

        virtual operator string() override;
};

class AddSubToken : public OpToken {
    public:
        AddSubToken(const string &r);
};

class MulDivToken : public OpToken {
    public:
        MulDivToken(const string &r);
};

class BracketToken : public OpToken {
    public:
        BracketToken(const string &r);
};
