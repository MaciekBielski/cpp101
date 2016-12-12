#include "operand.hpp"

Expression::Expression(TokenStack &filo, TokenStream &stream, CursesIO &io):
    stack{filo}, ts{stream}
{}

/* TODO: Thing about visitor here how to make it short */
void Expression::compute(const Token &t)
{
    t.compute(*this);
}

void run()
{
    stack.push(make_unique<ValToken>("0"s));
    stack.push(make_unique<AddSubToken>("+"s));

    auto notDone = bool {true};

    do {
        auto tokenPtr = ts.passToken(io);
        io.err("Passed: "s + static_cast<string>(*tokenPtr));
        compute(*tokenPtr);
    } while ();
}
