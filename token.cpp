/*
 * token.cpp
 */
#include "token.hpp"
#include "token_const.hpp"

namespace wasmgen
{
    /**/

    inline static UCharType get_char(CharType c)
    {
        return UCharType(c);
    }

    template <typename T>
    inline static UCharType get_char(const T& v)
    {
        return UCharType(v.first);
    }

    template <typename T>
    static void print_charset(const char* msg, const T& cs)
    {
        message("  ", msg, ":");
        for (const auto& p : cs)
        {
            UCharType c = get_char(p);
            if (0x20 < c && c <= 0x7e)
                message(" ", char(c));
            else
                message(" \\", CSBInt(int(c), 3, '0'));
        }
        message("\n");
    }

    ///////////
    // Token //
    ///////////

    Token::Token(TokenID id, FileString* text) noexcept
        : id(id)
        , text(text)
        , overflow(false)
        , isfloat(false)
        , ivalue(0)
        , fvalue(0)
        , missing_quote(false)
    {
        /*NOOP*/
    }

    /**/

    void Token::dump()
    {
        message(text->file_name->c_str(), ":", text->text_pos.line,
                ":TokenID=", CSBHex(unsigned(id) & 0xfff, 3, int('x')),
                ": ", int(id) == int('\n') ? "" : text->c_str(),
                "\n");

        if (id == TokenID::NUMBER)
            message("    ivalue: ", CSBHex(ivalue, 16, 'x'), ", ", ivalue, "\n"
                    "    fvalue: ", CSBHex(*(std::uint64_t*)&fvalue, 16, 'x'), ", ", fvalue, "\n",
                    "    fvalue: ", std::setprecision(80), fvalue, "\n");
        else if (id == TokenID::QUOTE)
            message("    quote[", missing_quote, "]: ", quote->c_str(), "\n");
    }

    /**/

    void Token::dump_charset()
    {
        message("CharSet:\n");
        print_charset("binary", TokenCharSet::binary);
        print_charset("octal", TokenCharSet::octal);
        print_charset("decimal", TokenCharSet::decimal);
        print_charset("hexadecimal", TokenCharSet::hexadecimal);

        print_charset("basic", TokenCharSet::basic);
        print_charset("control", TokenCharSet::control);
        print_charset("extend", TokenCharSet::extend);

        print_charset("space", TokenCharSet::space);
        print_charset("digit", TokenCharSet::digit);
        print_charset("upper", TokenCharSet::upper);
        print_charset("lower", TokenCharSet::lower);
        print_charset("alpha", TokenCharSet::alpha);
        print_charset("alnum", TokenCharSet::alnum);
        print_charset("allsym", TokenCharSet::allsym);

        print_charset("word1st", TokenCharSet::name1st);
        print_charset("word", TokenCharSet::name);
        print_charset("quote", TokenCharSet::quote);
        print_charset("symbol", TokenCharSet::symbol);
    }

} // wasmgen
