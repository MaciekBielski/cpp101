#pragma once

#include <memory>
#include <stack>

#include "token.hpp"
#include "token_stream.hpp"

using TokenStack = stack<unique_ptr<Token>>;

/* TODO: Setup promoters, served and degraders operators */
class Expression final {
    private:
        TokenStack &stack;
        TokenStream &ts;
    public:
        Expression() = delete;
        Expression(TokenStack&);

        void compute(unique_ptr<Token>);
        void run();
};
