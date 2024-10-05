/*
 * token_const.hpp
 */
#pragma once
#include "token.hpp"

namespace wasmgen
{
    ///////////////
    // TokenChar //
    ///////////////

    const CharMap TokenCharSet::binary {
        { '0', 0 }, { '1', 1 },
    };

    const CharMap TokenCharSet::octal {
        { '0', 0 }, { '1', 1 }, { '2', 2 }, { '3', 3 },
        { '4', 4 }, { '5', 5 }, { '6', 6 }, { '7', 7 },
    };

    const CharMap TokenCharSet::decimal {
        { '0', 0 }, { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 },
        { '5', 5 }, { '6', 6 }, { '7', 7 }, { '8', 8 }, { '9', 9 },
    };

    const CharMap TokenCharSet::hexadecimal {
        { '0', 0 }, { '1', 1 }, { '2', 2 }, { '3', 3 }, { '4', 4 },
        { '5', 5 }, { '6', 6 }, { '7', 7 }, { '8', 8 }, { '9', 9 },
        { 'A', 10 }, { 'B', 11 }, { 'C', 12 }, { 'D', 13 }, { 'E', 14 }, { 'F', 15 },
        { 'a', 10 }, { 'b', 11 }, { 'c', 12 }, { 'd', 13 }, { 'e', 14 }, { 'f', 15 },
    };

    /**/

    const CharSet TokenCharSet::basic(TokenCharSet::make(0x20, 0x7e));
    const CharSet TokenCharSet::control(TokenCharSet::make(0, 0x7f)
                                        - TokenCharSet::basic);
    const CharSet TokenCharSet::extend(TokenCharSet::make(0x80, 0xff));

    /**/

    const CharSet TokenCharSet::space { '\t', ' ' };
    const CharSet TokenCharSet::digit(TokenCharSet::make('0', '9'));
    const CharSet TokenCharSet::upper(TokenCharSet::make('A', 'Z'));
    const CharSet TokenCharSet::lower(TokenCharSet::make('a', 'z'));
    const CharSet TokenCharSet::alpha(TokenCharSet::upper
                                      + TokenCharSet::lower);
    const CharSet TokenCharSet::alnum(TokenCharSet::digit
                                      + TokenCharSet::alpha);

    const CharSet TokenCharSet::allsym(TokenCharSet::basic
                                       - TokenCharSet::alnum
                                       - TokenCharSet::space);

    /**/

    const CharSet TokenCharSet::name1st(BaseCharSet({ '$', '.', '_', '@' })
                                        + TokenCharSet::alpha);
    const CharSet TokenCharSet::name(TokenCharSet::name1st
                                     + TokenCharSet::digit);
    const CharSet TokenCharSet::quote { '\'', '"', '`' };
    const CharSet TokenCharSet::symbol(TokenCharSet::allsym
                                       - TokenCharSet::quote
                                       - TokenCharSet::name1st);

} // wasmgen
