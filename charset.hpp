#ifndef CHARSET_HPP
#define CHARSET_HPP

#include <string>
#include <set>

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
        inline bool isVal(const char &c) const;
        inline bool isAddOrSub(const char &c) const;
        inline bool isMulOrDiv(const char &c) const;
        inline bool isBracket(const char &c) const;

        static const CharSet& getInstance() {
            static const CharSet instance{};
            return instance;
        }
};


inline bool CharSet::isVal(const char &c) const
{
   return ( vals.find(c) != vals.end() );
}

inline bool CharSet::isAddOrSub(const char &c) const
{
   return ( addSub.find(c) != addSub.end() );
}

inline bool CharSet::isMulOrDiv(const char &c) const
{
   return ( mulDiv.find(c) != mulDiv.end() );
}

inline bool CharSet::isBracket(const char &c) const
{
   return ( brackets.find(c) != brackets.end() );
}

#endif /*CHARSET_HPP*/
