#ifndef CHARSET_HPP
#define CHARSET_HPP

#include <string>

using namespace std;

/* singleton */
class CharSet
{
    private:
        string valueChars  = {"0123456789."};
        string op1Chars    = {"+-"};
        string op2Chars    = {"*/"};
        string op3Chars    = {"()"};
        /* the only constructor */
        CharSet(){};
    public:
        const set<char> vals { valueChars.cbegin(), valueChars.cend() };
        const set<char> addSub { op1Chars.cbegin(), op1Chars.cend() };
        const set<char> mulDiv { op2Chars.cbegin(), op2Chars.cend() };
        const set<char> brackets { op3Chars.cbegin(), op3Chars.cend() };
        static const CharSet& getInstance() {
            static const CharSet instance{};
            return instance;
        }
};

#endif /*CHARSET_HPP*/
