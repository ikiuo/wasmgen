/*
 * expr.hpp
 */
#pragma once
#include "array.hpp"
#include "token.hpp"

namespace wasmgen
{
    ////////////////
    // Expression //
    ////////////////

    class Expression;

    using ExpressionPtr = Pointer<Expression>;
    using ExpressionRef = RefPointer<Expression>;
    using NewExpression = NewPointer<Expression>;

    using ExpressionList = PointerArray<Expression>;

    using ExpressionListPtr = Pointer<ExpressionList>;
    using ExpressionListRef = RefPointer<ExpressionList>;
    using NewExpressionList = NewPointer<ExpressionList>;

    using ExprIter = ExpressionList::iterator;
    using ExprConstIter = ExpressionList::const_iterator;

    /**/

    class Expression : virtual public Object
    {
    public:
        enum Mode
        {
            EMPTY,
            VALUE,
            UNARY,
            BINARY,
            CONDITIONAL,
            LIST,
        };

    protected:
        using OperatorPriority = StdMap<TokenID, int>;
        static const OperatorPriority binary_operator_priority;

    public:
        Mode mode;
        TokenPtr token;
        ExpressionListPtr children;
        TokenListPtr token_list;

    public:
        Expression() noexcept;
        Expression(Mode mode) noexcept;
        Expression(Token* value) noexcept;
        Expression(Token* op, Expression* value) noexcept;
        Expression(Token* op, Expression* lhs, Expression* rhs) noexcept;
        Expression(Token* op, Expression* cval, Expression* tval, Expression* fval) noexcept;
        Expression(Token* op, ExpressionList* list) noexcept;

        operator Token*() noexcept;
        operator const Token*() const noexcept;

        bool isnumber() noexcept;
        bool isname() noexcept;
        bool isquote() noexcept;
        bool isstring() noexcept;

        Token* gettoken() noexcept;
        String* getname() noexcept;
        String* getquote() noexcept;
        String* getstring() noexcept;

        const char *gettokencstr() noexcept;
        const char *getnamecstr() noexcept;
        const char *getstrcstr() noexcept;

        void dump(int indent = 0);

        static bool is_unary_operator(TokenID id) noexcept;
        static int priority(TokenID id) noexcept;
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////////
    // Expression //
    ////////////////

    inline Expression::operator Token*() noexcept
    {
        return token;
    }

    inline Expression::operator const Token*() const noexcept
    {
        return token;
    }

    /**/

    inline bool Expression::isnumber() noexcept
    {
        return mode == VALUE && token->id == TokenID::NUMBER;
    }

    inline bool Expression::isname() noexcept
    {
        return mode == VALUE && token->id == TokenID::NAME;
    }

    inline bool Expression::isquote() noexcept
    {
        return mode == VALUE && token->id == TokenID::QUOTE;
    }

    inline bool Expression::isstring() noexcept
    {
        return isname() || isquote();
    }

    inline Token* Expression::gettoken() noexcept
    {
        return (mode == BINARY || mode == CONDITIONAL)
            ? (*children)[0]->gettoken() : &token;
    }

    inline String* Expression::getname() noexcept
    {
        return isname() ? token->text : nullptr;
    }

    inline String* Expression::getquote() noexcept
    {
        return isquote() ? token->quote : nullptr;
    }

    inline String* Expression::getstring() noexcept
    {
        if (mode == VALUE)
        {
            switch (token->id)
            {
            case TokenID::NAME:
                return token->text;
            case TokenID::QUOTE:
                return token->quote;
            default:
                break;
            }
        }
        return nullptr;
    }

    inline const char *Expression::gettokencstr() noexcept
    {
        return GetCStr(gettoken());
    }

    inline const char *Expression::getnamecstr() noexcept
    {
        String* s = getname();

        return s ? s->c_str() : nullptr;
    }

    inline const char *Expression::getstrcstr() noexcept
    {
        String* s = getname();

        return s ? s->c_str() : nullptr;
    }

    /**/

    inline bool Expression::is_unary_operator(TokenID id) noexcept
    {
        return (id == TokenID('!') || id == TokenID('~') ||
                id == TokenID('+') || id == TokenID('-'));
    }

    inline int Expression::priority(TokenID id) noexcept
    {
        OperatorPriority::const_iterator it = binary_operator_priority.find(id);
        return it == binary_operator_priority.end() ? -1 : it->second;
    }

} // wasmgen
