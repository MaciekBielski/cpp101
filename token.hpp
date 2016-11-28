#pragma once

#include <string>

using namespace std;

class Token {
    private:
        const string raw;
    public:
        Token(const string& r);
};
