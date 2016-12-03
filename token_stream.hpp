#pragma once

#include <iostream>
#include <condition_variable>
#include <sstream>
#include <mutex>

#include "curses_io.hpp"
#include "token.hpp"

using namespace std;
using uint = unsigned int;

class TokenStream final {
    private:
        unique_ptr<Token> token = nullptr;
        std::mutex bufMtx;
        condition_variable bufCv;

        Token emitToken(const CursesIO &io, stringstream &acc);
    public:
        TokenStream(){};
        TokenStream(const TokenStream &) = delete;
        TokenStream(TokenStream &&) = delete;
        ~TokenStream(){};

        void parseInput(const CursesIO& io);
        void passToken(const CursesIO &io);
};
