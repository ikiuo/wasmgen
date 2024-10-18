/*
 * expression.cpp
 */
#include "expression.hpp"

namespace wasmgen
{
    ////////////////
    // Expression //
    ////////////////

    const Expression::UnaryOperator Expression::unary_operator {
        TokenID::BNOT, TokenID::NOT,
        TokenID::ADD, TokenID::SUB, TokenID::MUL,
    };

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

        { TokenID::VMUL, 9 },  // "[*]"
        { TokenID::VDIV, 9 },  // "[/]"
        { TokenID::VMOD, 9 },  // "[%]"

        { TokenID::VADD, 10 },  // "[+]"
        { TokenID::VSUB, 10 },  // "[-]"

        { TokenID::BAND, 11 },  // "&&"
        { TokenID::BOR, 12 },  // "||"
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

    Expression::Expression(Token* op, Expression* lhs, Expression* rhs, Mode mode) noexcept
        : mode(mode)
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

    void Expression::gettokenlist(TokenList* list)
    {
        if (paren_open)
            list->push_back(paren_open);
        switch (mode)
        {
        case EMPTY:
            break;
        case VALUE:
            list->push_back(token);
            break;

        case UNARY:
            list->push_back(token);
            (*children)[0]->gettokenlist(list);
            break;

        case BINARY:
            (*children)[0]->gettokenlist(list);
            list->push_back(token);
            (*children)[1]->gettokenlist(list);
            break;

        case CONDITIONAL:
            (*children)[0]->gettokenlist(list);
            list->push_back((*token_list)[0]);
            (*children)[1]->gettokenlist(list);
            list->push_back((*token_list)[1]);
            (*children)[2]->gettokenlist(list);
            break;

        case LIST:
            {
                size_t csz = children->size();
                size_t tsz = token_list->size();
                size_t nsz = std::min<size_t>(csz, tsz);

                for (auto n : inc_range<size_t>(nsz))
                {
                    (*children)[n]->gettokenlist(list);
                    list->push_back((*token_list)[n]);
                }
                if (tsz < csz)
                {
                    assert(csz == (tsz + 1));
                    (*children)[tsz]->gettokenlist(list);
                }
            }
            break;

        case ITEM:
            (*children)[0]->gettokenlist(list);
            (*children)[1]->gettokenlist(list);
            break;
        }
        if (paren_close)
            list->push_back(paren_close);
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

        case ITEM:
            message("ITEM:\n");
            n = children->size();
            break;
        }

        for (auto i : inc_range<size_t>(n))
            (*children)[i]->dump(indent);
    }

} // wasmgen
