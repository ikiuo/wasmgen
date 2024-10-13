/*
 * expression.cpp
 */
#include "expression.hpp"

namespace wasmgen
{
    ////////////////
    // Expression //
    ////////////////

    const Expression::OperatorPriority Expression::binary_operator_priority {
        { TokenID::MUL, 1 },  // "*"
        { TokenID::DIV, 1 },  // "/"
        { TokenID::MOD, 1 },  // "%"

        { TokenID::ADD, 2 },  // "+"
        { TokenID::SUB, 2 },  // "-"

        { TokenID::LSHIFT, 3 },   // "<<"
        { TokenID::RSHIFT, 3 },   // ">>"
        { TokenID::RSHIFTU, 3 },  // ">>>"

        { TokenID::CMPLT, 4 },  // "<"
        { TokenID::CMPLE, 4 },  // "<="
        { TokenID::CMPGE, 4 },  // ">="
        { TokenID::CMPGT, 4 },  // ">"

        { TokenID::CMPEQ, 5 },  // "=="
        { TokenID::CMPNE, 5 },  // "!="

        { TokenID::AND, 6 },  // "&"
        { TokenID::XOR, 7 },  // "^"
        { TokenID::OR,  8 },  // "|"

        { TokenID::BAND, 9 },  // "&&"
        { TokenID::BOR, 10 },  // "||"
    };

    /**/

    Expression::Expression() noexcept
        : mode(EMPTY)
        , children(new ExpressionList)
    {
        /*NOOP*/
    }

    Expression::Expression(Mode mode) noexcept
        : mode(mode)
        , children(new ExpressionList)
    {
        /*NOOP*/
    }

    Expression::Expression(Token* value) noexcept
        : mode(VALUE)
        , token(value)
        , children(new ExpressionList)
    {
        /*NOOP*/
    }

    Expression::Expression(Token* op, Expression* value) noexcept
        : mode(UNARY)
        , token(op)
        , children(new ExpressionList)
    {
        children->push_back(value);
    }

    Expression::Expression(Token* op, Expression* lhs, Expression* rhs) noexcept
        : mode(BINARY)
        , token(op)
        , children(new ExpressionList)
    {
        children->push_back(lhs);
        children->push_back(rhs);
    }

    Expression::Expression(Token* op, Expression* cval, Expression* tval, Expression* fval) noexcept
        : mode(CONDITIONAL)
        , token(op)
        , children(new ExpressionList)
    {
        children->push_back(cval);
        children->push_back(tval);
        children->push_back(fval);
    }

    Expression::Expression(Token* op, ExpressionList* list) noexcept
        : mode(LIST)
        , token(op)
        , children(list)
    {
        /*NOOP*/
    }

    /**/

    void Expression::dump(int indent)
    {
        String padding(indent++ * 4, ' ');
        size_t n = 0;

        message(padding);
        switch (mode)
        {
        case EMPTY:
            message("<EMPTY>\n");
            break;

        case VALUE:
            switch (token->id)
            {
            case TokenID::NUMBER:
                if (token->isfloat)
                    message("VALUE=", token->fvalue);
                else
                    message("VALUE=", token->ivalue);
                message(" : \"", GetCStr(token), "\"\n");
                break;
            case TokenID::NAME:
                message("NAME: \"", GetCStr(token), "\"\n");
                break;
            case TokenID::QUOTE:
                message("QUOTE: ", GetCStr(token), "\n");
                break;
            default:
                message("<UNKNOWN:", int(token->id), "> : \"", GetCStr(token), "\"\n");
                break;
            }
            break;

        case UNARY:
            message("UNARY='", GetCStr(token), "'\n");
            n = 1;
            break;

        case BINARY:
            message("BINARY='", GetCStr(token), "'\n");
            n = 2;
            break;

        case CONDITIONAL:
            message("CONDITIONAL:\n");
            n = 2;
            break;

        case LIST:
            n = children->size();
            message("LIST[", n, "]: '", GetCStr(token), "'\n");
            break;
        }

        for (auto i : inc_range<size_t>(n))
            (*children)[i]->dump(indent);
    }

} // wasmgen
