/*
 * token.hpp
 */
#pragma once
#include "array.hpp"
#include "file.hpp"

namespace wasmgen
{
    using BaseCharSet = StdSet<CharType>;
    using BaseCharMap = StdMap<CharType, int>;

    /////////////////
    // CharSetFlag //
    /////////////////

    template <typename T>
    class CharSetFlag : public T
    {
    private:
        using super = T;

    protected:
        using value_type = typename super::value_type;
        using initializer_list = std::initializer_list<value_type>;

        bool flag[256];

    public:
        CharSetFlag(initializer_list init);
        template <typename... Args> CharSetFlag(Args... ars);

        bool has(CharType c) const noexcept;
        bool has(UCharType c) const noexcept;

    protected:
        static UCharType getchar(CharType c) noexcept;
        template <typename U> static UCharType getchar(const U& p) noexcept;

        void initialize() noexcept;
    };

    using CharSet = CharSetFlag<BaseCharSet>;
    using CharMap = CharSetFlag<BaseCharMap>;

    //////////////////
    // TokenCharSet //
    //////////////////

    struct TokenCharSet
    {
        static const CharMap binary;
        static const CharMap octal;
        static const CharMap decimal;
        static const CharMap hexadecimal;

        static const CharSet basic;
        static const CharSet allsym;
        static const CharSet control;
        static const CharSet extend;

        static const CharSet space;
        static const CharSet digit;
        static const CharSet upper;
        static const CharSet lower;
        static const CharSet alpha;
        static const CharSet alnum;

        static const CharSet name1st;
        static const CharSet name;
        static const CharSet quote;
        static const CharSet symbol;

        static BaseCharSet make(int s, int e);
    };

    /////////////
    // TokenID //
    /////////////

    enum class TokenID
    {
        ERROR = -2,
        UNDEF = -1,

        /**/

        CHAR_NUL = 0,
        CHAR_SOH,
        CHAR_STX,
        CHAR_ETX,

        CHAR_EOT,
        CHAR_ENQ,
        CHAR_ACK,
        CHAR_BEL,

        CHAR_BS,
        CHAR_HT,
        CHAR_LF,
        CHAR_VT,

        CHAR_FF,
        CHAR_CR,
        CHAR_SO,
        CHAR_SI,

        CHAR_DLE,
        CHAR_DC1,
        CHAR_DC2,
        CHAR_DC3,

        CHAR_DC4,
        CHAR_NAK,
        CHAR_SYN,
        CHAR_ETB,

        CHAR_CAN,
        CHAR_EM,
        CHAR_SUB,
        CHAR_ESC,

        CHAR_FS,
        CHAR_GS,
        CHAR_RS,
        CHAR_LS,

        CHAR_SPACE,
        CHAR_EXCLAMATION_MARK,
        CHAR_QUOTATION_MARK,
        CHAR_NUMBER_SIGN,

        CHAR_DOLLAR_SIGN,
        CHAR_PERCENT_SIGN,
        CHAR_AMPERSAND,
        CHAR_APOSTROPHE,

        CHAR_LEFT_PARENTHESIS,
        CHAR_RIGHT_PARENTHESIS,
        CHAR_ASTERISK,
        CHAR_PLUS_SIGN,

        CHAR_COMMA,
        CHAR_PYPHEN_MINUS,
        CHAR_FULL_STOP,
        CHAR_SOLIDUS,

        CHAR_DIGIT_ZERO,
        CHAR_DIGIT_ONE,
        CHAR_DIGIT_TWO,
        CHAR_DIGIT_THREE,

        CHAR_DIGIT_FOUR,
        CHAR_DIGIT_FIVE,
        CHAR_DIGIT_SIX,
        CHAR_DIGIT_SEVEN,

        CHAR_DIGIT_EIGHT,
        CHAR_DIGIT_NINE,
        CHAR_COLON,
        CHAR_SEMICOLON,

        CHAR_LESS_THAN_SIGN,
        CHAR_EQUAL_SIGN,
        CHAR_GREATER_THAN_SIGN,
        CHAR_QUESTION_MARK,

        CHAR_COMMERCIAL_AT,
        CHAR_LATIN_CAPITAL_LETTER_A,
        CHAR_LATIN_CAPITAL_LETTER_B,
        CHAR_LATIN_CAPITAL_LETTER_C,

        CHAR_LATIN_CAPITAL_LETTER_D,
        CHAR_LATIN_CAPITAL_LETTER_E,
        CHAR_LATIN_CAPITAL_LETTER_F,
        CHAR_LATIN_CAPITAL_LETTER_G,

        CHAR_LATIN_CAPITAL_LETTER_H,
        CHAR_LATIN_CAPITAL_LETTER_I,
        CHAR_LATIN_CAPITAL_LETTER_J,
        CHAR_LATIN_CAPITAL_LETTER_K,

        CHAR_LATIN_CAPITAL_LETTER_L,
        CHAR_LATIN_CAPITAL_LETTER_M,
        CHAR_LATIN_CAPITAL_LETTER_N,
        CHAR_LATIN_CAPITAL_LETTER_O,

        CHAR_LATIN_CAPITAL_LETTER_P,
        CHAR_LATIN_CAPITAL_LETTER_Q,
        CHAR_LATIN_CAPITAL_LETTER_R,
        CHAR_LATIN_CAPITAL_LETTER_S,

        CHAR_LATIN_CAPITAL_LETTER_T,
        CHAR_LATIN_CAPITAL_LETTER_U,
        CHAR_LATIN_CAPITAL_LETTER_V,
        CHAR_LATIN_CAPITAL_LETTER_W,

        CHAR_LATIN_CAPITAL_LETTER_X,
        CHAR_LATIN_CAPITAL_LETTER_Y,
        CHAR_LATIN_CAPITAL_LETTER_Z,
        CHAR_LEFT_SQUARE_BRACKET,

        CHAR_YEN_SIGN,
        CHAR_RIGHT_SQUARE_BRACKET,
        CHAR_CIRCUMFLEX_ACCENT,
        CHAR_LOW_LINE,

        CHAR_GRAVE_ACCENT,
        CHAR_LATIN_SMALL_LETTER_A,
        CHAR_LATIN_SMALL_LETTER_B,
        CHAR_LATIN_SMALL_LETTER_C,

        CHAR_LATIN_SMALL_LETTER_D,
        CHAR_LATIN_SMALL_LETTER_E,
        CHAR_LATIN_SMALL_LETTER_F,
        CHAR_LATIN_SMALL_LETTER_G,

        CHAR_LATIN_SMALL_LETTER_H,
        CHAR_LATIN_SMALL_LETTER_I,
        CHAR_LATIN_SMALL_LETTER_J,
        CHAR_LATIN_SMALL_LETTER_K,

        CHAR_LATIN_SMALL_LETTER_L,
        CHAR_LATIN_SMALL_LETTER_M,
        CHAR_LATIN_SMALL_LETTER_N,
        CHAR_LATIN_SMALL_LETTER_O,

        CHAR_LATIN_SMALL_LETTER_P,
        CHAR_LATIN_SMALL_LETTER_Q,
        CHAR_LATIN_SMALL_LETTER_R,
        CHAR_LATIN_SMALL_LETTER_S,

        CHAR_LATIN_SMALL_LETTER_T,
        CHAR_LATIN_SMALL_LETTER_U,
        CHAR_LATIN_SMALL_LETTER_V,
        CHAR_LATIN_SMALL_LETTER_W,

        CHAR_LATIN_SMALL_LETTER_X,
        CHAR_LATIN_SMALL_LETTER_Y,
        CHAR_LATIN_SMALL_LETTER_Z,
        CHAR_LEFT_CURLY_BRACKET,

        CHAR_VERTICAL_LINE,
        CHAR_RIGHT_CURLY_BRACKET,
        CHAR_OVER_LINE,
        CHAR_DEL,

        /**/

        EXTEND,  // = 128

        /**/

        EOL = CHAR_LF,
        SPACE = CHAR_SPACE,

        CMPLT = CHAR_LESS_THAN_SIGN,
        EQUAL = CHAR_EQUAL_SIGN,
        CMPGT = CHAR_GREATER_THAN_SIGN,

        BNOT = CHAR_EXCLAMATION_MARK,

        NOT = CHAR_OVER_LINE,

        AND = CHAR_AMPERSAND,
        OR = CHAR_VERTICAL_LINE,
        XOR = CHAR_CIRCUMFLEX_ACCENT,

        ADD = CHAR_PLUS_SIGN,
        SUB = CHAR_PYPHEN_MINUS,
        MUL = CHAR_ASTERISK,
        DIV = CHAR_SOLIDUS,
        MOD = CHAR_PERCENT_SIGN,

        /**/

        END = 256,

        COMMENT,
        COMMENT_SPAN,
        CONTROL,
        NUMBER,
        NAME,
        QUOTE,

        LSHIFT,
        RSHIFT,
        RSHIFTU,

        CMPLE,
        CMPGE,
        CMPEQ,
        CMPNE,

        BAND,
        BOR,

        /**/
    };
    static_assert(int(TokenID::EXTEND) == 128, "Bug!!");

    ///////////
    // Token //
    ///////////

    class Token : virtual public Object
    {
    public:
        TokenID id;
        FileStringPtr text;

        bool overflow;
        bool isfloat;
        uint64_t ivalue;
        double fvalue;

        bool missing_quote;
        StringPtr quote;

    public:
        Token() noexcept;
        Token(TokenID id) noexcept;
        Token(TokenID id, FileString* text) noexcept;

        bool operator ==(CharType c) const noexcept;
        bool operator !=(CharType c) const noexcept;

        bool operator ==(UCharType c) const noexcept;
        bool operator !=(UCharType c) const noexcept;

        bool operator ==(TokenID id) const noexcept;
        bool operator !=(TokenID id) const noexcept;

        bool valid() const noexcept;
        template <typename T> void set_value(T value) noexcept;

        void dump();

        static void dump_charset();
    };

    using TokenPtr = Pointer<Token>;
    using TokenRef = RefPointer<Token>;
    using NewToken = NewPointer<Token>;

    /**/

    bool Valid(const Token* token) noexcept;
    bool Invalid(const Token* token) noexcept;
    bool IsSpace(const Token* token) noexcept;
    FileString* GetText(Token* token) noexcept;
    const FileString* GetText(const Token* token) noexcept;
    const char* GetCStr(const Token* token) noexcept;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    /////////////////
    // CharSetFlag //
    /////////////////

    template <typename T>
    inline CharSetFlag<T>::CharSetFlag(initializer_list il)
        : super(il.begin(), il.end())
    {
        initialize();
    }

    template <typename T>
    template <typename... Args>
    inline CharSetFlag<T>::CharSetFlag(Args... args)
        : super(args...)
    {
        initialize();
    }

    /**/

    template <typename T>
    inline bool CharSetFlag<T>::has(CharType c) const noexcept
    {
        return has(UCharType(c));
    }

    template <typename T>
    inline bool CharSetFlag<T>::has(UCharType c) const noexcept
    {
        return flag[c];
    }

    template <typename T>
    inline void CharSetFlag<T>::initialize() noexcept
    {
        std::memset(flag, 0, sizeof(flag));
        for (auto &d : *this)
            flag[getchar(d)] = true;
    }

    /**/

    template <typename T>
    inline UCharType CharSetFlag<T>::getchar(CharType c) noexcept
    {
        return UCharType(c);
    }

    template <typename T>
    template <typename U>
    inline UCharType CharSetFlag<T>::getchar(const U& p) noexcept
    {
        static_assert(sizeof(U::first) == sizeof(UCharType), "invalid type");
        return UCharType(p.first);
    }

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    //////////////////
    // TokenCharSet //
    //////////////////

    inline BaseCharSet TokenCharSet::make(int s, int e)
    {
        BaseCharSet rs;

        assert(s <= e && UCharType(s) == s && UCharType(e) == e);
        for (const auto c : inc_range<int>(s, e + 1))
            rs.insert(CharType(c));
        return rs;
    }

    ///////////
    // Token //
    ///////////


    inline Token::Token() noexcept
        : Token(TokenID::UNDEF, nullptr)
    {
        /*NOOP*/
    }

    inline Token::Token(TokenID id) noexcept
        : Token(id, nullptr)
    {
        /*NOOP*/
    }

    /**/

    inline bool Token::operator ==(CharType n) const noexcept
    {
        return int(n) == int(id);
    }

    inline bool Token::operator !=(CharType n) const noexcept
    {
        return int(n) != int(id);
    }

    inline bool Token::operator ==(UCharType n) const noexcept
    {
        return int(n) == int(id);
    }

    inline bool Token::operator !=(UCharType n) const noexcept
    {
        return int(n) != int(id);
    }

    inline bool Token::operator ==(TokenID n) const noexcept
    {
        return n == id;
    }

    inline bool Token::operator !=(TokenID n) const noexcept
    {
        return n != id;
    }

    /**/

    inline bool Token::valid() const noexcept
    {
        return int(id) >= 0;
    }

    template <typename T>
    inline void Token::set_value(T value) noexcept
    {
        isfloat = std::is_floating_point<T>::value;
        ivalue = uint64_t(value);
        fvalue = double(value);
    }

    ///////////

    inline bool Valid(const Token* token) noexcept
    {
        return token && token->valid();
    }

    inline bool Invalid(const Token* token) noexcept
    {
        return !Valid(token);
    }

    inline bool IsSpace(const Token* token) noexcept
    {
        return Valid(token) && token->id <= TokenID::CHAR_SPACE;
    }

    inline FileString* GetText(Token* token) noexcept
    {
        return token ? &token->text : nullptr;
    }

    inline const FileString* GetText(const Token* token) noexcept
    {
        return token ? &token->text : nullptr;
    }

    inline const char* GetCStr(const Token* token) noexcept
    {
        const FileString* text = GetText(token);

        return text ? text->c_str() : "(null)";
    }

} // wasmgen
