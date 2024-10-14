/*
 * parser.hpp
 */
#pragma once
#include "lexer.hpp"
#include "module.hpp"

namespace wasmgen
{
    ////////////
    // Parser //
    ////////////

    class Parser : public Lexer
    {
    protected:
        using MacroDataDict = StdStringMap<MacroDataPtr>;
        using SectionNameDict = StdStringMap<int8_t>;
        using ElementModeDict = StdStringMap<ElementMode>;
        using InstrMacroDict = StdStringMap<Instruction::MacroCode>;

        using PTokenList = StdVector<Token*>;

        using InstructionEntry = void (Parser::*)();
        using PseudoEntry = InstructionEntry;
        using PseudoDataEntry = InstructionEntry;
        using CloseSectionEntry = InstructionEntry;

        struct OpLT { template <typename L, typename R> inline bool op(L l, R r) { return l < r; } };
        struct OpLE { template <typename L, typename R> inline bool op(L l, R r) { return l <= r; } };
        struct OpEQ { template <typename L, typename R> inline bool op(L l, R r) { return l == r; } };
        struct OpNE { template <typename L, typename R> inline bool op(L l, R r) { return l != r; } };
        struct OpGE { template <typename L, typename R> inline bool op(L l, R r) { return l >= r; } };
        struct OpGT { template <typename L, typename R> inline bool op(L l, R r) { return l > r; } };

        struct OpAdd { template <typename L, typename R> inline auto op(L l, R r) { return l + r; } };
        struct OpSub { template <typename L, typename R> inline auto op(L l, R r) { return l - r; } };
        struct OpMul { template <typename L, typename R> inline auto op(L l, R r) { return l * r; } };
        struct OpDiv { template <typename L, typename R> inline auto op(L l, R r) { return l / r; } };
        struct OpMod { template <typename L, typename R> inline auto op(L l, R r) { return l % r; } };

        using Int64List = StdVector<int64_t>;

        struct GetTypeRes
        {
            String* name;
            ValType valtype;

            GetTypeRes() noexcept
                : name(nullptr), valtype(ValType::NONE) {}
            GetTypeRes(String* name, ValType valtype) noexcept
                : name(name), valtype(valtype) {}
        };

        using GetNameValTypeRes = GetTypeRes;

        struct ExprNest
        {
            int &value;

            ExprNest(int& v) : value(v) { ++value; };
            ~ExprNest() { --value; }
        };

        struct GetEqualRes
        {
            Token* name;
            ExprValue value;
        };

        struct ASMSWFlag {
            TokenPtr token;
            bool skip;
            bool skip_if;
            bool skip_else;
            bool set_else;

            ASMSWFlag() noexcept;
        };

        using ASMSWStack = StdVecStack<ASMSWFlag>;

    public:
        struct Option
        {
            int include_depth;
            bool comment_nest;
            bool section_datacount;
            bool type_unique;

            void set_default() noexcept;
        };

    protected:
        static const SectionNameDict section_name_dict;
        static const ElementModeDict element_mode_dict;
        static const InstrMacroDict instr_macro_dict;
        static const PseudoEntry pseudo_entry[Instruction::PSEUDO_SIZE];

        static SingletonString ins_dummy;

        static SingletonString ins_block;
        static SingletonString ins_else;
        static SingletonString ins_end;
        static SingletonString ins_if;
        static SingletonString ins_loop;

        static SingletonString ins_defmacro;

        static SingletonString ins_br_if;
        static SingletonString ins_return;

        static SingletonString ins_i32_const;
        static SingletonString ins_i64_const;
        static SingletonString ins_f32_const;
        static SingletonString ins_f64_const;
        static SingletonString ins_v128_const;

        static SingletonString optname_include_depth;
        static SingletonString optname_comment_nest;
        static SingletonString optname_section_datacount;
        static SingletonString optname_type_unique;

        static bool initialized_so;

        static InstrIter instr_dummy;

        static InstrIter instr_end;
        static InstrIter instr_if;

        static InstrIter instr_defmacro;

        static InstrIter instr_br_if;
        static InstrIter instr_return;

        static InstrIter instr_i32_const;
        static InstrIter instr_i64_const;
        static InstrIter instr_f32_const;
        static InstrIter instr_f64_const;
        static InstrIter instr_v128_const;

        static IdentifierPtr clset_name;

        Option option;
        ExpressionListPtr empty_operands;

        int error_count;
        bool retry;
        bool expr_error;
        int expr_nest;

        ASMSWStack asmsw_stack;
        ASMSWFlag asmsw_flag;

        MacroDataDict macro_dict;
        MacroDataPtr define_macro;
        StdSet<StdString> macro_expand;
        TextFileReaderPtr macro_file;

        CodeLinePtr code_line;
        CodeListPtr code_list;
        SectionPtr current_section;
        StringPtr current_module_name;

        InstructionEntry instruction_entry;
        PseudoDataEntry pseudo_data_entry;

        SectionListPtr section_list;

        IdentifierPtr boolean_name;
        IdentifierListPtr predefined_name;

        IdentifierPtr define_name;
        IdentifierPtr option_name;
        IdentifierListPtr asmsw_name;

        IdentifierPtr macro_name;
        IdentifierPtr alias_name;

        IdentifierListPtr global_name;
        IdentifierListPtr typeidx_name;
        IdentifierListPtr funcidx_name;
        IdentifierListPtr tableidx_name;
        IdentifierListPtr memidx_name;
        IdentifierListPtr globalidx_name;
        IdentifierListPtr elemidx_name;
        IdentifierListPtr dataidx_name;

        ModulePtr module;

        StringPtr current_directory;

    public:
        Parser(TextFileReader* reader) noexcept;

        bool parse();
        void build_wasm(ModuleBinary& wasm);

        void update_clset() noexcept;

        ExprValue get_asmsw_value(const char* name);

    protected:
        static void initialze_static_object();

        Token* next_token();
        Token* feed_eol();

        void clear_section();
        void next_section(SectionID id);

        bool check_operands(CodeLine* line, size_t min, size_t max) noexcept;

        bool parse_file();
        bool parse_file(String* curdir, TextFileReader* reader);
        bool parse_line();

        bool parse_macro();
        bool parse_macro_append();

        bool parse_operands();
        Expression* parse_expression();
        Expression* parse_expr_assignment();
        Expression* parse_expr_conditional();
        Expression* parse_expr_binary(int pop = 0);
        Expression* parse_expr_unary();
        Expression* parse_expr_value();
        Expression* parse_expr_list(TokenID cid, Token* st);

        void parse_code_line();
        void parse_invalid_line();

        /*-*/

        void parse_instruction();
        /**/
        void parse_pseudo_assemble_switch();
        void parse_pseudo_assemble_switch(CodeLine* line);
        /**/
        void parse_pseudo_include();
        void parse_pseudo_message();
        void parse_pseudo_option();
        /*-*/
        void parse_pseudo_macro_begin();
        void parse_pseudo_macro_end();
        void parse_pseudo_macro_delete();
        /**/
        void parse_pseudo_alias();
        /**/
        void parse_pseudo_empty();
        void parse_pseudo_binary();
        /**/
        template <SectionID id> void parse_pseudo_section();
        template <SectionID id> void parse_pseudo_section_instr();
        void parse_pseudo_custom();
        void parse_pseudo_data();
        /**/
        void parse_pseudo_import_parameter();
        void parse_pseudo_function_parameter();
        void parse_pseudo_element_parameter();
        void parse_pseudo_data_parameter();

        bool parse_pseudo_function_parameter(GetNameValTypeRes& res, CodeLine* code_line);

        /*-*/

        bool parse_section();
        /**/
        bool parse_custom_section();
        bool parse_custom_section(Section* sec);
        /**/
        bool parse_type_section();
        bool parse_type_section(Section* sec);
        bool parse_type_section_param(ResultType& restype, Identifier* idmap, CodeLine* line);
        bool parse_type_section_finish(Token* label, TypeData* data);
        /**/
        bool parse_import_section();
        bool parse_import_section(Section* sec);
        bool parse_import_section_module(CodeLine* line);
        bool parse_import_section_param(CodeLine* line);
        /**/
        bool parse_function_section();
        bool parse_function_section(Section* sec);
        bool parse_function_section_local(FunctionCode* func, CodeLineList& locals);
        /**/
        bool parse_table_section();
        bool parse_table_section(Section* sec);
        /**/
        bool parse_memory_section();
        bool parse_memory_section(Section* sec);
        /**/
        bool parse_global_section();
        bool parse_global_section(Section* sec);
        /**/
        bool parse_export_section();
        bool parse_export_section(Section* sec);
        /**/
        bool parse_start_section();
        /**/
        bool parse_element_section();
        bool parse_element_section(Section* sec);
        /**/
        bool parse_data_section();
        bool parse_data_section(Section* sec);

        /*-*/

        bool update_code_list();
        /**/
        bool update_custom_section();
        bool update_function_section();
        bool update_global_section();
        bool update_element_section();
        bool update_data_section();

        /*-*/

        void add_symbol(SectionID id, CodeList* list);

        bool reparse_code_list(CodeList* list);
        bool parse_code_list(CodeList* list);
        bool parse_code_line(CodeList* list, CodeLine* line);
        bool parse_code_line_operand(CodeList* list, CodeLine* line);
        bool parse_data_line_operand(CodeList* list, CodeLine* line);
        bool parse_data_line_binary(CodeList* list, CodeLine* line);

        /*-*/

        size_t add_functype(FuncType& functype);
        size_t add_blocktype(FuncType& functype);

        size_t append_index(Identifier* idmap, Token* token);
        void append_index(Identifier* idmap, Token* token, size_t index);

        bool update_br_idx(CodeList* list);

        template <typename T> void set_predefined(const char* name, T value);
        template <typename T> void update_option(String* name, Token* token, T value);
        bool update_option(String* name, Token* token, GetEqualRes& res,
                           Expression* op, String* optname, bool &option);
        bool update_option(String* name, Token* token, GetEqualRes& res,
                           Expression* op, String* optname, int &option,
                           int min = INT_MIN, int max = INT_MAX);
        void update_option() noexcept;

        /*-*/

        ExprValue getvalue(Expression* expr, IdentifierList* table = nullptr);
        ExprValue getvalue(StdStringSet& nest, Expression* expr, IdentifierList* table);

        String* getname(Expression* expr, IdentifierList* table = nullptr);
        String* getquote(Expression* expr, IdentifierList* table = nullptr);
        String* getstring(Expression* expr, IdentifierList* table = nullptr);
        String* getstring(StdStringSet& nest, Expression* expr, IdentifierList* table, bool text, bool quote);

        GetTypeRes gettype(const ValTypeDict& dict, Expression* expr, IdentifierList* table = nullptr);

        bool getequal(GetEqualRes& res, Expression* expr, IdentifierList* table = nullptr);
        bool geteqval(GetEqualRes& res, Expression* expr, IdentifierList* table = nullptr);
        bool geteqstr(GetEqualRes& res, Expression* expr, IdentifierList* table = nullptr);

        bool getreftype(GetTypeRes& res, Expression* expr, IdentifierList* table = nullptr);
        bool getvaltype(GetTypeRes& res, Expression* expr, IdentifierList* table = nullptr);
        bool getvaltypes(ResultType& res, ExpressionList* ops);
        bool getlimits(Limits& limits, CodeLine* line, size_t pos, IdentifierList* table = nullptr);
        bool getfunctype(FuncType& res, Expression* expr);
        bool getfunctype_resulttypes(ResultType& res, Expression* expr);

        TypeData* gettypedata(Expression* expr);

        /*-*/

        template <typename... Args> Token* make_token(Token* token, Args... args);

        bool make_expr(CodeList* code, Token* token, ValType valtype, int opstart, ExprConstIter opb, ExprConstIter ope);
        bool make_expr(CodeList* code, Token* token, const StdString& instr, InstrIter instab, int opstart, ExprConstIter opb, ExprConstIter ope);

        /*-*/

        template <typename F> static ExprValue binary_operator(ExprValue& l, ExprValue& r, F op);
        template <typename T> static Expression* make_value(Token* token, T value);
        template <typename T> static Expression* make_value(FileString* text, T value);
        static Expression* make_string(Token* token, String* quote);
        static Expression* make_string(FileString* text, String* quote);

        /*-*/

        StdString make_error_message(ErrorCode ecode, const char* prefix, const PTokenList& token_list);

        void parse_message(const Token* token, const StringBlock& msg);
        template <typename ...Args> void parse_message(const Token* token, Args... args);

        bool parse_error(ErrorCode code, const PTokenList& token_list);
        bool parse_expr_error(ErrorCode code, const PTokenList& token_list);
        void parse_warning(ErrorCode code, const PTokenList& token_list);
        void parse_warning_too_many_operands(CodeLine* line, Token* token, size_t n);
        void parse_warning_too_many_operands(CodeLine* line, ExpressionList* ops, size_t n);

        [[noreturn]] void parse_bug_unknown_pseudo_code(CodeLine* line);

        /*-*/

        void put_custom_section(ModuleBinary& wasm, SectionID id);

        void put_custom_section(ModuleBinary& wasm);
        void put_type_section(ModuleBinary& wasm);
        void put_import_section(ModuleBinary& wasm);
        void put_function_section(ModuleBinary& wasm);
        void put_table_section(ModuleBinary& wasm);
        void put_memory_section(ModuleBinary& wasm);
        void put_global_section(ModuleBinary& wasm);
        void put_export_section(ModuleBinary& wasm);
        void put_start_section(ModuleBinary& wasm);
        void put_element_section(ModuleBinary& wasm);
        void put_code_section(ModuleBinary& wasm);
        void put_data_section(ModuleBinary& wasm);
        void put_datacount_section(ModuleBinary& wasm);
    };

    using ParserPtr = Pointer<Parser>;
    using ParserRef = RefPointer<Parser>;
    using NewParser = NewPointer<Parser>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // Parser //
    ////////////

    inline void Parser::update_clset() noexcept
    {
        clset_name = define_name;
    }

    /*
     *
     */

    template <SectionID id>
    void Parser::parse_pseudo_section()
    {
        next_section(id);
    }

    template <SectionID id>
    void Parser::parse_pseudo_section_instr()
    {
        next_section(id);
        instruction_entry = &Parser::parse_code_line;
    }

    inline void Parser::parse_pseudo_assemble_switch()
    {
        parse_pseudo_assemble_switch(code_line);
    }

    /*
     *
     */

    inline ExprValue Parser::getvalue(Expression* expr, IdentifierList* table)
    {
        StdStringSet nest;

        return getvalue(nest, expr, table ? table : &global_name);
    }

    inline String* Parser::getname(Expression* expr, IdentifierList* table)
    {
        StdStringSet nest;

        return getstring(nest, expr, table ? table : &global_name, true, false);
    }

    inline String* Parser::getquote(Expression* expr, IdentifierList* table)
    {
        StdStringSet nest;

        return getstring(nest, expr, table ? table : &global_name, false, true);
    }

    inline String* Parser::getstring(Expression* expr, IdentifierList* table)
    {
        StdStringSet nest;

        return getstring(nest, expr, table ? table : &global_name, true, true);
    }

    inline Parser::GetTypeRes Parser::gettype(const ValTypeDict& dict, Expression* expr, IdentifierList* table)
    {
        String* name = getname(expr, table ? table : &global_name);

        return GetTypeRes(name, name ? dict.get(*name) : ValType::NONE);
    }

    /**/

    inline size_t Parser::append_index(Identifier* idmap, Token* token)
    {
        size_t index = idmap->size();

        append_index(idmap, token, index);
        return index;
    }

    template <typename T>
    void Parser::set_predefined(const char* text, T value)
    {
        NewFileString name(text);
        ExpressionRef expr(make_value(name, value));

        (*boolean_name)[*name] = Transfer(expr);
    }

    template <typename T>
    void Parser::update_option(String* name, Token* token, T value)
    {
        (*option_name)[*name] = make_value(token, value);

        WASMGEN_DEBUG(2, "option: ", name->c_str(), "=", value, "\n");
    }

    /*
     *
     */

    template <typename... Args>
    Token* Parser::make_token(Token* token, Args... args)
    {
        FileString* text = token->text;
        auto s = new FileString(StringBlock{args...});

        s->file_name = text->file_name;
        s->text_pos = text->text_pos;
        return new Token(TokenID::NUMBER, s);
    }

    /**/

    template <typename F>
    inline ExprValue Parser::binary_operator(ExprValue& l, ExprValue& r, F f)
    {
        assert(l.isnumber() && r.isnumber());
        return (!r.isfloat()
                ? (!l.isfloat()
                   ? ExprValue(f.op(l.ivalue, r.ivalue))
                   : ExprValue(f.op(l.fvalue, r.ivalue)))
                : (!l.isfloat()
                   ? ExprValue(f.op(l.ivalue, r.fvalue))
                   : ExprValue(f.op(l.fvalue, r.fvalue))));
    }

    /**/

    template <typename T>
    inline Expression* Parser::make_value(Token* token, T value)
    {
        return make_value(token->text, value);
    }

    template <typename T>
    Expression* Parser::make_value(FileString* text, T value)
    {
        Token* token = new Token(TokenID::NUMBER, text);

        token->set_value(value);
        return NewRef<Expression>(token);
    }

    inline Expression* Parser::make_string(Token* token, String* quote)
    {
        return make_string(token->text, quote);
    }

    inline Expression* Parser::make_string(FileString* text, String* quote)
    {
        Token* token = new Token(TokenID::QUOTE, text);

        token->quote = quote;
        return NewRef<Expression>(token);
    }

    /*
     *
     */

    inline void Parser::parse_message(const Token* token, const StringBlock& msg)
    {
        token_message(token, msg);
    }

    template <typename ...Args>
    inline void Parser::parse_message(const Token* token, Args... args)
    {
        token_message(token, args...);
    }

    inline void Parser::parse_warning_too_many_operands(CodeLine* line, ExpressionList* ops, size_t n)
    {
        parse_warning_too_many_operands(line, *(*ops)[n], n);
    }

    inline void Parser::parse_bug_unknown_pseudo_code(CodeLine* line)
    {
        throw BUG("未知の擬似命令 \"", GetCStr(line->instr), "\" です。");
    }

    /*
     *
     */

    inline void Parser::put_custom_section(ModuleBinary& wasm)
    {
        put_custom_section(wasm, SectionID::CUSTOM);
    }

    ///////////////
    // ASMSWFlag //
    ///////////////

    inline Parser::ASMSWFlag::ASMSWFlag() noexcept
        : skip()
        , skip_if()
        , skip_else()
        , set_else()
    {
        /*NOOP*/
    }

} // wasmgen
