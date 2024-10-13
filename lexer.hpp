/*
 * lexer.hpp
 */
#pragma once
#include "array.hpp"
#include "token.hpp"

namespace wasmgen
{
    ///////////
    // Lexer //
    ///////////

    class Lexer : virtual public Object
    {
    protected:
        using TokenStack = ObjectStack<Token>;
        using TokenEntry = void (Lexer::*)(UCharType c);

        enum
        {
            token_entry_size = 1 << (sizeof(UCharType) << 3),
        };

        struct TokenData
        {
            TokenID id;
            StdString text;
        };

        using TokenDataList = StdVector<TokenData>;

        struct ScanTree
        {
            struct {
                TokenID id;
                ScanTree* next;
            } code[token_entry_size];

            ScanTree();
            ScanTree(const TokenDataList& list);
            void clear();
        };

        TextFileReaderPtr freader;
        FileStringListPtr file_line;
        FileTextPos text_pos;
        FileTextPos save_pos;

        ObjectStack<TextFileReader> freader_stack;
        ObjectStack<FileStringList> line_stack;
        StdVecStack<FileTextPos> pos_stack;

        TokenListPtr token_line;
        bool token_eol;

        TokenStack token_stack;
        FileStringPtr current_text;
        TokenPtr current_token;

        StdVector<char> message_buffer;

        bool nestable_comment;
        bool nested_comments;
        bool asmsw_skip;

        TokenEntry token_entry[token_entry_size];

        static const TokenDataList token_data;
        static const ScanTree token_scan_tree;

    public:
        Lexer(TextFileReader* reader) noexcept;

        Token* gettoken();
        void puttoken(Token* token) noexcept;

    protected:
        void prepare(const CharSet& cs, TokenEntry entry) noexcept;
        void prepare(const CharMap& cs, TokenEntry entry) noexcept;

        bool isspace(UCharType c) noexcept;
        bool isdigit(UCharType c) noexcept;
        bool isalpha(UCharType c) noexcept;
        bool islchar(UCharType c) noexcept;
        bool isnchar(UCharType c) noexcept;

        bool fetchar();
        UCharType getchar() noexcept;
        void rewindchar() noexcept;

        void prepare_token();

        template <typename T>
        void gettoken_chars(const T& cs);

        void gettoken_char(UCharType c);

        void gettoken_control(UCharType c);
        void gettoken_symchar(UCharType c);
        void gettoken_extend(UCharType c);

        void gettoken_cr(UCharType c);
        void gettoken_lf(UCharType c);

        void gettoken_space(UCharType c);
        void gettoken_comment(UCharType c);
        void gettoken_comment_start(UCharType c);
        void gettoken_comment_cstyle(UCharType c);

        void gettoken_name(UCharType c);
        void gettoken_number(UCharType c);
        void gettoken_scan_tree(UCharType c);

        void gettoken_quote(UCharType c);
        void gettoken_quote_escape(UCharType c);
        void gettoken_quote_escape_ochar(UCharType c);
        void gettoken_quote_escape_uchar(UCharType c);
        void gettoken_quote_escape_xchar(UCharType c);
        void gettoken_quote_escape_xchar_sub(UCharType c, bool ucs);

        void token_id(TokenID id) noexcept;
        void token_id(TokenID id, UCharType c) noexcept;
        void token_append(UCharType c);

        void token_message(const Token* token, const char* msg);
        void token_message(const Token* token, const StringBlock &msg);
        void token_error(ErrorCode code);

        void push_reader(TextFileReader* reader);
        void pop_reader();
    };

    using LexerPtr = Pointer<Lexer>;
    using LexerRef = RefPointer<Lexer>;
    using NewLexer = NewPointer<Lexer>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ///////////
    // Lexer //
    ///////////

    inline void Lexer::prepare(const CharSet& cs, TokenEntry entry) noexcept
    {
        for (const auto i : cs)
            token_entry[size_t(i)] = entry;
    }

    inline void Lexer::prepare(const CharMap& cs, TokenEntry entry) noexcept
    {
        for (const auto p : cs)
            token_entry[size_t(p.first)] = entry;
    }

    /**/

    inline void Lexer::puttoken(Token* token) noexcept
    {
        token_stack.push(token);
    }

    /**/

    inline bool Lexer::isspace(UCharType c) noexcept
    {
        return c == '\t' || c == ' ';
    }

    inline bool Lexer::isdigit(UCharType c) noexcept
    {
        return '0' <= c && c <= '9';
    }

    inline bool Lexer::isalpha(UCharType c) noexcept
    {
        return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
    }

    inline bool Lexer::islchar(UCharType c) noexcept
    {
        return isdigit(c) || isalpha(c) || c == '_';
    }

    inline bool Lexer::isnchar(UCharType c) noexcept
    {
        return islchar(c) || c == '.';
    }

    /**/

    inline UCharType Lexer::getchar() noexcept
    {
        save_pos = text_pos;

        assert(text_pos.line < file_line->size());

        FileString *curr = (*file_line)[text_pos.line];

        return UCharType(curr->at(text_pos.column++));
    }

    inline void Lexer::rewindchar() noexcept
    {
        text_pos = save_pos;
    }

    template <typename T>
    void Lexer::gettoken_chars(const T& cs)
    {
        while (fetchar())
        {
            UCharType c = getchar();

            if (!cs.has(c))
            {
                rewindchar();
                break;
            }
            token_append(c);
        }
    }

    /**/

    inline void Lexer::gettoken_quote_escape_uchar(UCharType c)
    {
        gettoken_quote_escape_xchar_sub(c, true);
    }

    inline void Lexer::gettoken_quote_escape_xchar(UCharType c)
    {
        gettoken_quote_escape_xchar_sub(c, false);
    }

    /**/

    inline void Lexer::token_id(TokenID id) noexcept
    {
        current_token->id = id;
    }

    inline void Lexer::token_id(TokenID id, UCharType c) noexcept
    {
        token_id(id);
        token_append(c);
    }

    inline void Lexer::token_append(UCharType c)
    {
        current_text->push_back(CharType(c));
    }

    /**/

    inline void Lexer::token_message(const Token* token, const StringBlock &msg)
    {
        token_message(token, msg.c_str());
    }

} // wasmgen
