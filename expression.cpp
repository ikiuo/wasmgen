/*
 * expression.cpp
 */
#include "expression.hpp"

namespace wasmgen
{
    ////////////////
    // Expression //
    ////////////////

    enum : int
    {
        XPRI_NONE,

        XPRI_POWER,
        XPRI_MUL,
        XPRI_ADD,

        XPRI_SHLR,

        XPRI_CMPR,
        XPRI_CMPE,

        XPRI_AND,
        XPRI_XOR,
        XPRI_OR,

        XPRI_BAND,
        XPRI_BOR,

        /**/

        XPRI_VPOW,

        XPRI_VMUL,
        XPRI_VADD,

        XPRI_VSHLR,

        XPRI_VCMPR,
        XPRI_VCMPE,

        XPRI_VAND,
        XPRI_VXOR,
        XPRI_VOR,

        XPRI_VBAND,
        XPRI_VBOR,
    };

    const Expression::UnaryOperator Expression::unary_operator {
        TokenID::BNOT, TokenID::NOT,
        TokenID::ADD, TokenID::SUB, TokenID::MUL,
    };

    const Expression::OperatorPriority Expression::binary_operator_priority {
        { TokenID::POWER, XPRI_POWER },  // "**"

        { TokenID::MUL, XPRI_MUL },  // "*"
        { TokenID::DIV, XPRI_MUL },  // "/"
        { TokenID::MOD, XPRI_MUL },  // "%"

        { TokenID::ADD, XPRI_ADD },  // "+"
        { TokenID::SUB, XPRI_ADD },  // "-"

        { TokenID::LSHIFT,  XPRI_SHLR },   // "<<"
        { TokenID::RSHIFT,  XPRI_SHLR },   // ">>"
        { TokenID::RSHIFTU, XPRI_SHLR },  // ">>>"

        { TokenID::CMPLT, XPRI_CMPR },  // "<"
        { TokenID::CMPLE, XPRI_CMPR },  // "<="
        { TokenID::CMPGE, XPRI_CMPR },  // ">="
        { TokenID::CMPGT, XPRI_CMPR },  // ">"

        { TokenID::CMPEQ, XPRI_CMPE },  // "=="
        { TokenID::CMPNE, XPRI_CMPE },  // "!="

        { TokenID::AND, XPRI_AND },  // "&"
        { TokenID::XOR, XPRI_XOR },  // "^"
        { TokenID::OR,  XPRI_OR },  // "|"

        { TokenID::BAND, XPRI_BAND },  // "&&"
        { TokenID::BOR,  XPRI_BOR },  // "||"

        /**/

        { TokenID::VPOW, XPRI_VPOW },  // "[**]"

        { TokenID::VMUL, XPRI_VMUL },  // "[*]"
        { TokenID::VDIV, XPRI_VMUL },  // "[/]"
        { TokenID::VMOD, XPRI_VMUL },  // "[%]"

        { TokenID::VADD, XPRI_VADD },  // "[+]"
        { TokenID::VSUB, XPRI_VADD },  // "[-]"

        { TokenID::VSHL,  XPRI_VSHLR },   // "[<<]"
        { TokenID::VSHR,  XPRI_VSHLR },   // "[>>]"
        { TokenID::VSHRU, XPRI_VSHLR },  // "[>>>]"

        { TokenID::VCMPLT, XPRI_VCMPR },  // "[<]"
        { TokenID::VCMPLE, XPRI_VCMPR },  // "[<=]"
        { TokenID::VCMPGE, XPRI_VCMPR },  // "[>=]"
        { TokenID::VCMPGT, XPRI_VCMPR },  // "[>]"

        { TokenID::VCMPEQ, XPRI_VCMPE },  // "[==]"
        { TokenID::VCMPNE, XPRI_VCMPE },  // "[!=]"

        { TokenID::VAND, XPRI_VAND },  // "[&]"
        { TokenID::VXOR, XPRI_VXOR },  // "[^]"
        { TokenID::VOR,  XPRI_VOR },  // "[|]"

        { TokenID::VBAND, XPRI_VBAND },  // "[&&]"
        { TokenID::VBOR,  XPRI_VBOR },  // "[||]"
    };

    /**/

    Expression::Expression() noexcept
        : mode(EMPTY)
        , children(new ExpressionList)
        , from_range(false)
    {
        /*NOOP*/
    }

    Expression::Expression(Mode mode) noexcept
        : mode(mode)
        , children(new ExpressionList)
        , from_range(false)
    {
        /*NOOP*/
    }

    Expression::Expression(Token* value) noexcept
        : mode(VALUE)
        , token(value)
        , children(new ExpressionList)
        , from_range(false)
    {
        /*NOOP*/
    }

    Expression::Expression(Token* op, Expression* value) noexcept
        : mode(UNARY)
        , token(op)
        , children(new ExpressionList)
        , from_range(false)
    {
        children->push_back(value);
    }

    Expression::Expression(Token* op, Expression* lhs, Expression* rhs, Mode mode) noexcept
        : mode(mode)
        , token(op)
        , children(new ExpressionList)
        , from_range(false)
    {
        children->push_back(lhs);
        children->push_back(rhs);
    }

    Expression::Expression(Token* op, Expression* cval, Expression* tval, Expression* fval) noexcept
        : mode(CONDITIONAL)
        , token(op)
        , children(new ExpressionList)
        , from_range(false)
    {
        children->push_back(cval);
        children->push_back(tval);
        children->push_back(fval);
    }

    Expression::Expression(Token* op, ExpressionList* list) noexcept
        : mode(LIST)
        , token(op)
        , children(list)
        , from_range(false)
    {
        /*NOOP*/
    }

    /**/

    void Expression::gettokenlist(TokenList* list)
    {
        assert(list);

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
        case RANGE:
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

    bool Expression::setlistseparator(TokenList* separators, bool comma)
    {
        if (!separators || !separators->size())
            return false;
        setlistseparator((*separators)[0], comma);
        return true;
    }

    void Expression::setlistseparators(size_t count, Token* separator, bool comma)
    {
        if (comma)
            separator = make_comman(separator);
        token_list = new TokenList(count ? count - 1 : 0, separator);
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
            message("UNARY[", int(token->id), "]='", GetCStr(token), "'\n");
            n = 1;
            break;

        case BINARY:
            message("BINARY[", int(token->id), "]='", GetCStr(token), "'\n");
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

        case RANGE:
            n = children->size();
            message("RANGE[", n, "]: '", GetCStr(token), "'\n");
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
