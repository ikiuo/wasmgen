/*
 * lexer.cpp
 */
#include "lexer.hpp"

namespace wasmgen
{
    ///////////
    // Lexer //
    ///////////

    const Lexer::TokenDataList Lexer::token_data {
        { TokenID::LSHIFT, "<<" },
        { TokenID::RSHIFT, ">>" },
        { TokenID::RSHIFTU, ">>>" },
        { TokenID::CMPLE, "<=" },
        { TokenID::CMPGE, ">=" },
        { TokenID::CMPEQ, "==" },
        { TokenID::CMPNE, "!=" },
        { TokenID::BAND, "&&" },
        { TokenID::BOR, "||" },
    };

    const Lexer::ScanTree Lexer::token_scan_tree(token_data);

    /**/

    Lexer::Lexer(TextFileReader* reader) noexcept
        : freader()
        , text_pos(0, 0)
        , save_text_pos(0, 0)
    {
        static_assert(sizeof(UCharType) == 1, "invalid type");

        push_reader(reader);

        for (auto i : inc_range<size_t>(token_entry_size))
            token_entry[i] = &Lexer::gettoken_char;

        prepare(TokenCharSet::control, &Lexer::gettoken_control);
        prepare(TokenCharSet::space, &Lexer::gettoken_space);
        prepare(TokenCharSet::digit, &Lexer::gettoken_number);
        prepare(TokenCharSet::name1st, &Lexer::gettoken_name);
        prepare(TokenCharSet::quote, &Lexer::gettoken_quote);

        token_entry[size_t('\r')] = &Lexer::gettoken_cr;
        token_entry[size_t('\n')] = &Lexer::gettoken_lf;

        token_entry[size_t('#')] = &Lexer::gettoken_comment;
        token_entry[size_t(';')] = &Lexer::gettoken_comment;
        token_entry[size_t('.')] = &Lexer::gettoken_number;

        for (auto i : inc_range<size_t>(token_entry_size))
            if (token_scan_tree.code[i].next)
                token_entry[size_t(i)] = &Lexer::gettoken_scan_tree;
    }

    /**/

    bool Lexer::fetchar()
    {
        assert(text_pos.line <= file_line.size());
        if (text_pos.line >= file_line.size())
        {
            FileStringRef line = freader->readline();
            CharType LF = '\n';

            if (!line)
                return false;

            if (line->back() != LF)
                line->push_back(LF);
            file_line.push_back(line);
            text_pos.column = 0;
            return true;
        }
        if (text_pos.column < file_line[text_pos.line]->size())
            return true;
        ++text_pos.line;
        return fetchar();
    }

    /**/

    Token* Lexer::gettoken()
    {
        if (!freader)
            return nullptr;
        if (token_stack)
            return token_stack.pop();
        if (!fetchar())
            return nullptr;
        prepare_token();

        UCharType c = getchar();

        (this->*token_entry[c])(c);
        return Finish(current_token);
    }

    /**/

    void Lexer::prepare_token()
    {
        current_text = new FileString("");
        current_token = new Token(TokenID::UNDEF, current_text);

        FileString *line = file_line[text_pos.line];

        current_text->file_name = freader->path();
        current_text->text_pos.set(line->text_pos.line, text_pos.column);
    }

    void Lexer::gettoken_char(UCharType c)
    {
        token_id(TokenID(c), c);
    }

#define GETTOKEN_CHAR(func, id)                 \
    void Lexer::gettoken_##func(UCharType c)    \
    {                                           \
        token_id(TokenID::id, c);               \
    }

    GETTOKEN_CHAR(control, CONTROL)
    GETTOKEN_CHAR(lf, EOL)

#undef GETTOKEN_CHAR

    /**/

#define GETTOKEN_CHARS(charset, id)             \
    void Lexer::gettoken_##charset(UCharType c) \
    {                                           \
        token_id(TokenID::id, c);               \
        gettoken_chars(TokenCharSet::charset);  \
    }

    GETTOKEN_CHARS(space, SPACE)
    GETTOKEN_CHARS(name, NAME)
    GETTOKEN_CHARS(extend, EXTEND)

#undef GETTOKEN_CHARS

    /**/

    void Lexer::gettoken_comment(UCharType c)
    {
        token_id(TokenID::COMMENT, c);
        while (fetchar())
        {
            c = getchar();
            if (c == '\n')
            {
                rewindchar();
                break;
            }
            token_append(c);
        }
    }

    void Lexer::gettoken_cr(UCharType c)
    {
        token_id(TokenID::CONTROL, c);
        if (fetchar())
        {
            c = getchar();
            if (c == '\n')
            {
                token_id(TokenID::EOL, c);
                return;
            }
        }
        rewindchar();
    }

    void Lexer::gettoken_quote(UCharType c)
    {
        String* qs = new String("");
        UCharType qc = c;
        bool missing = true;

        token_id(TokenID::QUOTE, c);
        current_token->quote = qs;
        while (fetchar())
        {
            c = getchar();
            if (c == '\n')
            {
                rewindchar();
                break;
            }
            if (c == '\\')
            {
                gettoken_quote_escape(c);
                continue;
            }
            token_append(c);
            if (c == qc)
            {
                missing = false;
                break;
            }
            qs->push_back(CharType(c));
        }
        current_token->missing_quote = missing;
    }

    void Lexer::gettoken_quote_escape(UCharType c)
    {
        token_append(c);
        if (!fetchar())
            return;

        String* qs = current_token->quote;
        UCharType ec = c;

        c = getchar();
        switch (c)
        {
        case 'a': ec = '\a'; break;
        case 'b': ec = '\b'; break;
        case 't': ec = '\t'; break;
        case 'n': ec = '\n'; break;
        case 'v': ec = '\v'; break;
        case 'f': ec = '\r'; break;

        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
            gettoken_quote_escape_ochar(c);
            return;
        case 'U': case 'u':
            gettoken_quote_escape_uchar(c);
            return;
        case 'X': case 'x':
            gettoken_quote_escape_xchar(c);
            return;

        default:
            break;
        }
        token_append(c);
        qs->push_back(CharType(ec));
    }

    void Lexer::gettoken_quote_escape_ochar(UCharType c)
    {
        const CharMap& ochar = TokenCharSet::octal;
        CharMap::const_iterator it;

        int value = ochar.at(CharType(c));
        int len = 1;

        token_append(c);
        for (; len < 3 && fetchar(); ++len)
        {
            c = getchar();
            it = ochar.find(CharType(c));
            if (it == ochar.end())
            {
                rewindchar();
                break;
            }
            token_append(c);
            value = value * 8 + it->second;
        }
        if (0 <= value && value < 256)
            current_token->quote->push_back(CharType(value));
        else
            token_error(ErrorCode::INVALID_OCTAL_CODE);
    }

    void Lexer::gettoken_quote_escape_xchar_sub(UCharType c, bool ucs)
    {
        const CharMap& xchar = TokenCharSet::hexadecimal;
        CharMap::const_iterator it;

        String* qs = current_token->quote;
        std::uint32_t value = 0;
        int maxlen = ucs ? 6 : 2;
        int len = 0;

        token_append(c);
        for (; len < maxlen && fetchar(); ++len)
        {
            c = getchar();
            it = xchar.find(CharType(c));
            if (it == xchar.end())
            {
                rewindchar();
                break;
            }
            token_append(c);
            value = value * 16 + it->second;
        }
        if (!len)
        {
            qs->push_back(CharType(c));
            token_error(ErrorCode::INVALID_ESCAPE_SEQUENCE);
            return;
        }
        if (ucs)
            utfcvt::utf::putcode(*qs, value);
        else
            qs->push_back(CharType(value));
    }

    void Lexer::gettoken_number(UCharType c)
    {
        token_id(TokenID::NUMBER, c);
        gettoken_chars(TokenCharSet::name);

        String* text = current_token->text;
        size_t tsize = text->size();
        String::const_iterator sit = text->begin();
        String::const_iterator eit = text->end();

        std::uint64_t ivalue = 0;
        std::uint64_t nvalue = 0;
        double fvalue = 0;
        double fexp = 1;

        const CharMap *cmap = &TokenCharSet::decimal;
        CharMap::const_iterator nit;

        bool sdot = (c == '.');
        bool dot = sdot;
        bool overflow = false;
        int base = 10;
        int column = 0;
        int fraction = 0;
        bool sexp = false;
        int exp = 0;
        int cnum = 0;

        if (dot)
        {
            if (++sit == eit)
                goto set_name;
        }
        else if (c == '0')
        {
            if (++sit == eit)
                goto set_value;

            c = UCharType(*sit);
            switch (c)
            {
            case '0': case '1': case '2': case '3':
            case '4': case '5': case '6': case '7':
            case 'O': case 'o':
                cmap = &TokenCharSet::octal;
                base = 8;
                break;

            case 'B': case 'b':
                cmap = &TokenCharSet::binary;
                base = 2;
                ++sit;
                break;

            case 'D': case 'd':
                ++sit;
                break;

            case 'X': case 'x':
                cmap = &TokenCharSet::hexadecimal;
                base = 16;
                ++sit;
                break;

            case '.':
            case '_':
                break;

            default:
                goto error;
            }
        }

        for (; sit != eit; ++sit)
        {
            c = UCharType(*sit);
            if (c == '_')
                continue;
            if (c == '.')
            {
                if (dot)
                    goto fallback;
                dot = true;
                continue;
            }
            nit = cmap->find(c);
            if (nit == cmap->end())
                break;
            cnum = nit->second;
            if (!dot)
            {
                nvalue = ivalue * base + cnum;
                if (ivalue > nvalue)
                    overflow = true;
                ivalue = nvalue;

                fvalue = fvalue * base + cnum;
            }
            else
            {
                fvalue = fvalue * base + cnum;
                ++fraction;
            }
            ++column;
        }
        if (!column)
            goto fallback;
        if (sit == eit)
        {
            if (fraction)
                fvalue /= std::pow(base, fraction);
            goto set_value;
        }

        c = UCharType(*sit);
        if (!((base == 10 && (c == 'E' || c == 'e')) ||
              (base == 16 && (c == 'P' || c == 'p'))))
            goto fallback;
        assert(base == 10 || base == 16);
        if (++sit == eit)
        {
            if (!fetchar())
                goto error;
            c = getchar();
            if (c == '+' || c == '-')
                token_append(c);
            else
            {
                rewindchar();
                goto error;
            }
            gettoken_chars(TokenCharSet::name);

            sit = text->begin() + tsize;
            eit = text->end();
        }
        c = UCharType(*sit);
        if (c == '+')
        {
            if (++sit == eit)
                goto error;
        }
        else if (c == '-')
        {
            sexp = true;
            if (++sit == eit)
                goto error;
        }

        column = 0;
        for (; sit != eit; ++sit)
        {
            c = UCharType(*sit);
            nit = TokenCharSet::decimal.find(CharType(c));
            if (nit == cmap->end())
                goto error;
            exp = exp * 10 + nit->second;
            ++column;
        }
        if (!column)
            goto error;
        if (sexp)
            exp = -exp;
        if (fraction)
            exp -= (base == 10) ? fraction : fraction * 4;
        if (exp)
        {
            fexp = std::pow(base == 10 ? 10 : 2, exp);
            fvalue *= fexp;
            ivalue = std::uint64_t(ivalue * fexp);
            if (fvalue > (std::numeric_limits<std::uint64_t>::max)())
                overflow = true;
        }
        dot = true;

    set_value:
        current_token->overflow = overflow;
        current_token->isfloat = dot;
        current_token->ivalue = ivalue;
        current_token->fvalue = fvalue;
        return;
    fallback:
        if (!sdot)
            goto error;
    set_name:
        token_id(TokenID::NAME);
        return;
    error:
        token_error(ErrorCode::INVALID_NUMBER_FORMAT);
        return;
    }

    void Lexer::gettoken_scan_tree(UCharType c)
    {
        FileTextPos save_pos = text_pos;
        TokenID save_id = TokenID(c);
        size_t pending_pos = 0;

        constexpr size_t pending_size = 32;
        CharType pending_char[pending_size];
        size_t pending_count = 0;

        token_id(TokenID(c), c);

        auto node = token_scan_tree.code[UCharType(c)].next;

        while (fetchar())
        {
            c = getchar();

            auto& code = node->code[UCharType(c)];

            if (pending_count >= pending_size)
                throw BUG("バッファが足りなかった:", pending_count, " >= ", pending_size);

            pending_char[pending_count++] = c;
            if (code.id != TokenID::UNDEF)
            {
                save_pos = text_pos;
                save_id = code.id;
                pending_pos = pending_count;
            }
            if (!(node = code.next))
                break;
        }

        for (auto i : inc_range<size_t>(pending_pos))
            token_append(pending_char[i]);

        text_pos = save_pos;
        token_id(save_id);
    }

    /**/

    void Lexer::token_message(const Token* token, const char* msg)
    {
        const FileString* text = GetText(token);
        auto fname = freader->c_path();
        auto line = text_pos.line;

        if (text)
        {
            fname = text->file_name->c_str();
            line = text->text_pos.line;
        }
        message(fname, ":", line, ":", msg, "\n");
    }

    void Lexer::token_error(ErrorCode code)
    {
        auto& ect = error_code_table[code];

        token_id(TokenID::ERROR);
        token_message(current_token, {"error:T", unsigned(code), ": ", ect.messages[0]});
    }

    /**/

    void Lexer::push_reader(TextFileReader* reader)
    {
        assert(reader);
        if (freader)
            freader_stack.push_back(freader);
        freader = reader;
    }

    void Lexer::pop_reader()
    {
        if (freader_stack.size())
        {
            freader = freader_stack.back();
            freader_stack.pop_back();
        }
    }

    //////////////
    // ScanTree //
    //////////////

    void Lexer::ScanTree::clear()
    {
        for (auto i : inc_range<size_t>(token_entry_size))
        {
            code[i].id = TokenID::UNDEF;
            code[i].next = nullptr;
        }
    }

    Lexer::ScanTree::ScanTree()
    {
        clear();
    }

    Lexer::ScanTree::ScanTree(const TokenDataList& list)
    {
        clear();

        for (auto& data : list)
        {
            auto& text = data.text;
            auto pit = text.begin();
            auto eit = text.end();
            auto p = this;

            while (pit != eit)
            {
                auto& code = p->code[UCharType(*pit)];
                auto n = code.next;

                ++pit;
                if (!n)
                {
                    if (pit == eit)
                    {
                        code.id = data.id;
                        break;
                    }
                    code.next = n = new ScanTree;
                }
                p = n;

                assert(pit != eit);
            }
        }
    }

} // wasmgen
