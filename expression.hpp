/*
 * expr.hpp
 */
#pragma once
#include "array.hpp"
#include "token.hpp"

namespace wasmgen
{
    class Expression;

    using ExpressionPtr = Pointer<Expression>;
    using ExpressionRef = RefPointer<Expression>;
    using NewExpression = NewPointer<Expression>;

    ////////////////////
    // ExpressionList //
    ////////////////////

    using ExpressionList = PointerArray<Expression>;

    using ExpressionListPtr = Pointer<ExpressionList>;
    using ExpressionListRef = RefPointer<ExpressionList>;
    using NewExpressionList = NewPointer<ExpressionList>;

    using ExprIter = ExpressionList::iterator;
    using ExprConstIter = ExpressionList::const_iterator;

    ////////////////
    // Expression //
    ////////////////

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
            RANGE,
            ITEM,
        };

    protected:
        using OperatorPriority = StdMap<TokenID, int>;
        using UnaryOperator = StdSet<TokenID>;

        static const UnaryOperator unary_operator;
        static const OperatorPriority binary_operator_priority;

    public:
        Mode mode;
        TokenPtr token;
        ExpressionListPtr children;
        TokenListPtr token_list;
        TokenPtr paren_open;
        TokenPtr paren_close;
        TokenPtr list_separator;
        bool from_range;

    public:
        Expression() noexcept;
        Expression(Mode mode) noexcept;
        Expression(Token* value) noexcept;
        Expression(Token* op, Expression* value) noexcept;
        Expression(Token* op, Expression* lhs, Expression* rhs, Mode mode = BINARY) noexcept;
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

        void gettokenlist(TokenList* list);

        void setlistparen(Token* token);
        void setlistparen(Expression* expr);
        void setlistform(Expression* expr);
        void setlistseparator(Token* separator, bool comma = false);
        bool setlistseparator(TokenList* separators, bool comma = false);
        void setlistseparators(size_t count, Token* separator, bool comma = false);

        void dump(int indent = 0);

        static bool is_unary_operator(TokenID id) noexcept;
        static int priority(TokenID id) noexcept;
        static Token* make_char(Token* token, TokenID id);
        static Token* make_comman(Token* separator);
        static Token* make_lbracket(Token* separator);
        static Token* make_rbracket(Token* separator);
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
        switch (mode)
        {
        case BINARY:
        case CONDITIONAL:
        case ITEM:
            return (*children)[0]->gettoken();
        default:
            break;
        }
        return token;
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

    inline void Expression::setlistparen(Token* token)
    {
        paren_open = make_lbracket(token);
        paren_close = make_rbracket(token);
    }

    inline void Expression::setlistparen(Expression* expr)
    {
        paren_open = expr->paren_open;
        paren_close = expr->paren_close;
    }

    inline void Expression::setlistform(Expression* expr)
    {
        setlistparen(expr);
        list_separator = expr->list_separator;
    }

    inline void Expression::setlistseparator(Token* separator, bool comma)
    {
        assert(separator);

        list_separator = comma ? make_comman(separator) : separator;
    }

    /**/

    inline bool Expression::is_unary_operator(TokenID id) noexcept
    {
        return unary_operator.find(id) != unary_operator.end();
    }

    inline int Expression::priority(TokenID id) noexcept
    {
        return binary_operator_priority.get(id, -1);
    }

    inline Token* Expression::make_char(Token* token, TokenID id)
    {
        return token->id == id ? token : new Token(id, token->text);
    }

    inline Token* Expression::make_comman(Token* sep)
    {
        return make_char(sep, TokenID::CHAR_COMMA);
    }

    inline Token* Expression::make_lbracket(Token* sep)
    {
        return make_char(sep, TokenID::CHAR_LEFT_CURLY_BRACKET);
    }

    inline Token* Expression::make_rbracket(Token* sep)
    {
        return make_char(sep, TokenID::CHAR_RIGHT_CURLY_BRACKET);
    }

} // wasmgen
