/*
 * parser.cpp
 */
#include "parser.hpp"

namespace wasmgen
{
    ////////////
    // Parser //
    ////////////

    const Parser::SectionNameDict Parser::section_name_dict = {
        { "custom"   ,  0 }, { "customsec"   ,  0 },
        { "type"     ,  1 }, { "typesec"     ,  1 },
        { "import"   ,  2 }, { "importsec"   ,  2 },
        { "function" ,  3 }, { "funcsec"     ,  3 }, { "func", 3 },
        { "table"    ,  4 }, { "tablesec"    ,  4 },
        { "memory"   ,  5 }, { "memsec"      ,  5 }, { "mem", 5 },
        { "global"   ,  6 }, { "globalsec"   ,  6 },
        { "export"   ,  7 }, { "exportsec"   ,  7 },
        { "start"    ,  8 }, { "startsec"    ,  8 },
        { "element"  ,  9 }, { "elemsec"     ,  9 }, { "elem", 9 },
        { "code"     , 10 }, { "codesec"     , 10 },
        { "data"     , 11 }, { "datasec"     , 11 },
        { "datacount", 12 }, { "datacountsec", 12 },
    };

    /**/

    const Parser::ElementModeDict Parser::element_mode_dict = {
        { "active",      ElementMode::ACTIVE      },
        { "declarative", ElementMode::DECLARATIVE },
        { "passive",     ElementMode::PASSIVE     },
    };

    /**/

    const Parser::InstrMacroDict Parser::instr_macro_dict = {
        { "i32.eqz", Instruction::MC_COND_BR },
        { "s32.eqz", Instruction::MC_COND_BR },
        { "u32.eqz", Instruction::MC_COND_BR },
        { "i64.eqz", Instruction::MC_COND_BR },
        { "s64.eqz", Instruction::MC_COND_BR },
        { "u64.eqz", Instruction::MC_COND_BR },

        { "i32.eq", Instruction::MC_COND_BR },
        { "s32.eq", Instruction::MC_COND_BR },
        { "u32.eq", Instruction::MC_COND_BR },
        { "i64.eq", Instruction::MC_COND_BR },
        { "s64.eq", Instruction::MC_COND_BR },
        { "u64.eq", Instruction::MC_COND_BR },
        { "f32.eq", Instruction::MC_COND_BR },
        { "f64.eq", Instruction::MC_COND_BR },

        { "i32.ne", Instruction::MC_COND_BR },
        { "s32.ne", Instruction::MC_COND_BR },
        { "u32.ne", Instruction::MC_COND_BR },
        { "i64.ne", Instruction::MC_COND_BR },
        { "s64.ne", Instruction::MC_COND_BR },
        { "u64.ne", Instruction::MC_COND_BR },
        { "f32.ne", Instruction::MC_COND_BR },
        { "f64.ne", Instruction::MC_COND_BR },

        { "i32.lt_s", Instruction::MC_COND_BR },
        { "s32.lt", Instruction::MC_COND_BR },
        { "i32.lt_u", Instruction::MC_COND_BR },
        { "u32.lt", Instruction::MC_COND_BR },
        { "i64.lt_s", Instruction::MC_COND_BR },
        { "s64.lt", Instruction::MC_COND_BR },
        { "i64.lt_u", Instruction::MC_COND_BR },
        { "u64.lt", Instruction::MC_COND_BR },
        { "f32.lt", Instruction::MC_COND_BR },
        { "f64.lt", Instruction::MC_COND_BR },

        { "i32.le_s", Instruction::MC_COND_BR },
        { "s32.le", Instruction::MC_COND_BR },
        { "i32.le_u", Instruction::MC_COND_BR },
        { "u32.le", Instruction::MC_COND_BR },
        { "i64.le_s", Instruction::MC_COND_BR },
        { "s64.le", Instruction::MC_COND_BR },
        { "i64.le_u", Instruction::MC_COND_BR },
        { "u64.le", Instruction::MC_COND_BR },
        { "f32.le", Instruction::MC_COND_BR },
        { "f64.le", Instruction::MC_COND_BR },

        { "i32.ge_s", Instruction::MC_COND_BR },
        { "s32.ge", Instruction::MC_COND_BR },
        { "i32.ge_u", Instruction::MC_COND_BR },
        { "u32.ge", Instruction::MC_COND_BR },
        { "i64.ge_s", Instruction::MC_COND_BR },
        { "s64.ge", Instruction::MC_COND_BR },
        { "i64.ge_u", Instruction::MC_COND_BR },
        { "u64.ge", Instruction::MC_COND_BR },
        { "f32.ge", Instruction::MC_COND_BR },
        { "f64.ge", Instruction::MC_COND_BR },

        { "i32.gt_s", Instruction::MC_COND_BR },
        { "s32.gt", Instruction::MC_COND_BR },
        { "i32.gt_u", Instruction::MC_COND_BR },
        { "u32.gt", Instruction::MC_COND_BR },
        { "i64.gt_s", Instruction::MC_COND_BR },
        { "s64.gt", Instruction::MC_COND_BR },
        { "i64.gt_u", Instruction::MC_COND_BR },
        { "u64.gt", Instruction::MC_COND_BR },
        { "f32.gt", Instruction::MC_COND_BR },
        { "f64.gt", Instruction::MC_COND_BR },
    };

    /**/

    const Parser::PseudoEntry Parser::pseudo_entry[Instruction::PSEUDO_SIZE] = {
        &Parser::parse_instruction,
        &Parser::parse_instruction,
        /**/
        &Parser::parse_pseudo_assemble_switch,
        &Parser::parse_pseudo_include,
        &Parser::parse_pseudo_message,
        &Parser::parse_pseudo_option,
        /**/
        &Parser::parse_pseudo_macro_begin,
        &Parser::parse_pseudo_macro_end,
        &Parser::parse_pseudo_macro_delete,
        /**/
        &Parser::parse_pseudo_alias,
        /**/
        /**/
        &Parser::parse_pseudo_empty,
        &Parser::parse_pseudo_binary,
        /**/
        &Parser::parse_pseudo_custom,
        &Parser::parse_pseudo_section<SectionID::TYPE>,
        &Parser::parse_pseudo_section<SectionID::IMPORT>,
        &Parser::parse_pseudo_section_instr<SectionID::FUNCTION>,
        &Parser::parse_pseudo_section<SectionID::TABLE>,
        &Parser::parse_pseudo_section<SectionID::MEMORY>,
        &Parser::parse_pseudo_section_instr<SectionID::GLOBAL>,
        &Parser::parse_pseudo_section<SectionID::EXPORT>,
        &Parser::parse_pseudo_section<SectionID::EXPORT>,  // FUNCTION
        &Parser::parse_pseudo_section<SectionID::EXPORT>,  // TABLE
        &Parser::parse_pseudo_section<SectionID::EXPORT>,  // MEMORY
        &Parser::parse_pseudo_section<SectionID::EXPORT>,  // GLOBAL
        &Parser::parse_pseudo_section<SectionID::START>,
        &Parser::parse_pseudo_section_instr<SectionID::ELEMENT>,
        &Parser::parse_pseudo_data,
        &Parser::parse_pseudo_section<SectionID::DATACOUNT>,
        /**/
        &Parser::parse_pseudo_import_parameter,
        &Parser::parse_pseudo_function_parameter,
        &Parser::parse_pseudo_element_parameter,
        &Parser::parse_pseudo_data_parameter,
    };

    /**/

    SingletonString Parser::ins_dummy("");

    SingletonString Parser::ins_block("block");
    SingletonString Parser::ins_loop("loop");
    SingletonString Parser::ins_if("if");
    SingletonString Parser::ins_else("else");
    SingletonString Parser::ins_end("end");

    SingletonString Parser::ins_defmacro("defmacro");

    SingletonString Parser::ins_br_if("br_if");
    SingletonString Parser::ins_return("return");

    SingletonString Parser::ins_i32_const("i32.const");
    SingletonString Parser::ins_i64_const("i64.const");
    SingletonString Parser::ins_f32_const("f32.const");
    SingletonString Parser::ins_f64_const("f64.const");
    SingletonString Parser::ins_v128_const("v128.const");

    /**/

    SingletonString Parser::optname_include_depth("include.depth");
    SingletonString Parser::optname_paren_multiline("paren.multiline");
    SingletonString Parser::optname_comment_nest("comment.nest");
    SingletonString Parser::optname_section_datacount("section.datacount");
    SingletonString Parser::optname_type_unique("type.unique");

    /**/

    bool Parser::initialized_so = false;

    InstrIter Parser::instr_dummy;

    InstrIter Parser::instr_block;
    InstrIter Parser::instr_loop;
    InstrIter Parser::instr_if;
    InstrIter Parser::instr_else;
    InstrIter Parser::instr_end;

    InstrIter Parser::instr_defmacro;

    InstrIter Parser::instr_br_if;
    InstrIter Parser::instr_return;

    InstrIter Parser::instr_i32_const;
    InstrIter Parser::instr_i64_const;
    InstrIter Parser::instr_f32_const;
    InstrIter Parser::instr_f64_const;
    InstrIter Parser::instr_v128_const;

    IdentifierPtr Parser::clset_name;

    /**/

    void Parser::initialze_static_object()
    {
        instr_dummy = Instruction::table.find(*ins_dummy);

        instr_block = Instruction::table.find(*ins_block);
        instr_loop = Instruction::table.find(*ins_loop);
        instr_if = Instruction::table.find(*ins_if);
        instr_else = Instruction::table.find(*ins_else);
        instr_end = Instruction::table.find(*ins_end);

        instr_defmacro = Instruction::table.find(*ins_defmacro);

        instr_br_if = Instruction::table.find(*ins_br_if);
        instr_return = Instruction::table.find(*ins_return);

        instr_i32_const = Instruction::table.find(*ins_i32_const);
        instr_i64_const = Instruction::table.find(*ins_i64_const);
        instr_f32_const = Instruction::table.find(*ins_f32_const);
        instr_f64_const = Instruction::table.find(*ins_f64_const);
        instr_v128_const = Instruction::table.find(*ins_v128_const);
    }

    /**/

    Parser::Parser(TextFileReader* reader) noexcept
        : Lexer(reader)
          /**/
        , empty_operands(new ExpressionList)
          /**/
        , error_count()
        , retry()
          /**/
        , expr_error()
        , expr_nest()
        , expr_optimize()
          /**/
        , macro_file(new TextFileReader(new FileData))
          /**/
        , instruction_entry(&Parser::parse_invalid_line)
        , pseudo_data_entry(&Parser::parse_invalid_line)
          /**/
        , section_list(new SectionList)
          /**/
        , boolean_name(new Identifier)
        , predefined_name(new IdentifierList)
          /**/
        , define_name(new Identifier)
        , option_name(new Identifier)
        , asmsw_name(new IdentifierList)
          /**/
        , macro_name(new Identifier)
        , alias_name(new Identifier)
          /**/
        , global_name(new IdentifierList)
        , typeidx_name(new IdentifierList)
        , funcidx_name(new IdentifierList)
        , tableidx_name(new IdentifierList)
        , memidx_name(new IdentifierList)
        , globalidx_name(new IdentifierList)
        , elemidx_name(new IdentifierList)
        , dataidx_name(new IdentifierList)
          /**/
        , module(new Module)
          /**/
        , current_directory()
    {
        if (!initialized_so)
        {
            initialze_static_object();
            initialized_so = true;
        }

        option.set_default();
        update_option();

        predefined_name->push_back(boolean_name);
        {
            set_predefined("true", true);
            set_predefined("false", false);

            set_predefined("on", true);
            set_predefined("off", false);

            set_predefined("yes", true);
            set_predefined("no", false);
        }

        asmsw_name->push_back(boolean_name);
        asmsw_name->push_back(option_name);
        asmsw_name->push_back(define_name);
        asmsw_name->push_back(alias_name);

        global_name->push_back(alias_name);

        typeidx_name->push_back(module->typeidx);
        typeidx_name->push_back(alias_name);

        funcidx_name->push_back(module->funcidx);
        funcidx_name->push_back(alias_name);

        tableidx_name->push_back(module->tableidx);
        tableidx_name->push_back(alias_name);

        memidx_name->push_back(module->memidx);
        memidx_name->push_back(alias_name);

        globalidx_name->push_back(module->globalidx);
        globalidx_name->push_back(alias_name);

        elemidx_name->push_back(module->elemidx);
        elemidx_name->push_back(alias_name);

        dataidx_name->push_back(module->dataidx);
        dataidx_name->push_back(alias_name);

        {
            StringRef curdir(FileString::updatedir(FileString::dir_dot, reader->path()));

            current_directory = Transfer(curdir);
        }
    }

    /**/

    Token* Parser::next_token()
    {
        for (;;)
        {
            TokenRef rt = gettoken();

            if (Invalid(rt))
                return Finish(rt);

            switch (rt->id)
            {
            case TokenID::EOL:
                if (!skip_eol)
                    return Finish(rt);
                break;

            case TokenID::COMMENT:
                return feed_eol();

            case TokenID::COMMENT_SPAN:
                if (nested_comments && !nestable_comment)
                    parse_warning(ErrorCode::NESTED_COMMENTS, {rt});
                break;

            default:
                if (!IsSpace(rt))
                    return Finish(rt);
                break;
            }
        }
    }

    Token* Parser::feed_eol()
    {
        for (;;)
        {
            TokenRef rt = gettoken();

            if (Invalid(rt))
                return Finish(rt);
            if (rt->id == TokenID::EOL)
                return Finish(rt);
        }
    }

    /*
     *
     */

    void Parser::clear_section()
    {
        code_list.clear();
        current_section.clear();
        instruction_entry = &Parser::parse_invalid_line;
        pseudo_data_entry = &Parser::parse_invalid_line;
    }

    void Parser::next_section(SectionID id)
    {
        auto sec = new Section(id);

        section_list->push_back(sec);

        code_list = sec->code;
        current_section = sec;
        instruction_entry = &Parser::parse_invalid_line;
        pseudo_data_entry = &Parser::parse_invalid_line;

        parse_code_line();
    }

    /*
     *
     */

    bool Parser::check_operands(CodeLine* line, size_t min, size_t max) noexcept
    {
        ExpressionList* ops = line->operands; assert(ops);

        if (ops->size() < min)
            return parse_error_too_few_operands(line);
        if (ops->size() > max)
            parse_warning_too_many_operands(line, ops, max);
        return true;
    }

    Parser::InstrRes Parser::find_instr(String* name, bool noalias) noexcept
    {
        InstrRes res {
            Instruction::table.end(),
            macro_dict.end(),
            false,
        };

        res.instr = Instruction::table.find(*name);
        if (res.instr != Instruction::table.end())
        {
            res.find = true;
            return res;
        }
        res.macro = macro_dict.find(*name);
        if (res.macro != macro_dict.end())
        {
            res.find = true;
            return res;
        }
        if (noalias)
            return res;

        auto it = alias_name->find(*name);

        if (it == alias_name->end())
            return res;

        Expression* expr = it->second; assert(expr);

        if (expr->mode != Expression::VALUE)
            return res;

        Token* et = expr->token; assert(et);

        if (et->id != TokenID::NAME)
            return res;
        return find_instr(et->text, true);
    }

    /*
     *
     */

    bool Parser::parse()
    {
        if (clset_name)
            for (auto it = clset_name->begin(); it != clset_name->end(); ++it)
                (*define_name)[it->first] = it->second;

        clear_section();
        if (!parse_file())
            return false;
        if (asmsw_stack.size())
            parse_warning(ErrorCode::ASSEMBLE_SWITCH_NO_ENDIF, {asmsw_flag.token});
        clear_section();
        if (!parse_section())
            return false;
        clear_section();

        for (int pass = 1; pass <= 4; ++pass)
        {
            WASMGEN_DEBUG(2, "pass - ", pass, "\n");

            if (!update_code_list())
                return false;
            if (!retry)
                return true;
        }
        return parse_error(ErrorCode::ASSEMBLY_FAILED, {});
    }

    bool Parser::parse_file()
    {
        while (parse_line())
            ; /*NOOP*/

        if (define_macro)
            define_macro = nullptr;
        return !error_count;
    }

    bool Parser::parse_file(String* curdir, TextFileReader* reader)
    {
        StringPtr olddir = current_directory;

        current_directory = curdir;
        push_reader(reader);

        bool success = parse_file();

        pop_reader();
        current_directory = olddir;
        return success;
    }

    bool Parser::parse_line()
    {
        auto eit = Instruction::table.end();
        auto meit = macro_dict.end();

        asmsw_skip = asmsw_flag.skip;
        code_line = new CodeLine;

        if (flag_debug >= 3 && alt_token_stack->size())
        {
            StdString mtext;

            for (auto sit = alt_token_stack->rbegin();
                 sit != alt_token_stack->rend() &&
                     (*sit)->id != TokenID::EOL;
                 ++sit)
                mtext += *(*sit)->text;
            message("MACRO-EXPAND: ", mtext, "\n");
        }

        /**/

        TokenRef rt;

        for (;;)
        {
            rt = next_token();
            if (Invalid(rt))
                return !asmsw_skip ? false : Valid(rt = feed_eol());
            if (!IsSpace(rt))
                break;
        }

        if (rt->id != TokenID::NAME)
        {
            if (!asmsw_skip)
                return parse_error(ErrorCode::SYNTAX_ERROR, {rt});
            return Valid(rt = feed_eol());
        }

        InstrRes insres = find_instr(rt->text);

        if (!insres.find)
        {
            if (asmsw_skip)
                return Valid(rt = feed_eol());

            code_line->label = rt;
            rt = next_token();
            if (Valid(rt) && rt->id == TokenID(':'))
                rt = next_token();
            if (Valid(rt) && rt->id == TokenID::NAME)
                insres = find_instr(rt->text);
        }

        auto it = insres.instr;
        auto mit = insres.macro;

        if (insres.find)
        {
            if (asmsw_skip)
            {
                auto& operation = it->second.operation;

                if (operation != Instruction::PSEUDO_ASSMBLE_SWITCH)
                    return Valid(rt = feed_eol());
                assert(operation != Instruction::ASW_NONE);
            }

            code_line->instr = rt;
            if (it == eit)
                it = instr_defmacro;
            code_line->instab = it;
            if (!(expr_optimize = mit == meit))
                code_line->macro = mit->second;
            if (!parse_operands())
                return false;
            rt = next_token();
        }
        if (Valid(rt) && rt->id != TokenID::EOL)
        {
            TokenRef eol = feed_eol();

            if (define_macro)
                return parse_macro_append();
            parse_error(ErrorCode::SYNTAX_ERROR, {rt});
            if (code_line->label)
                parse_message(rt, " \"", GetCStr(code_line->label), "\" を確認してください。");
            return Valid(eol);
        }

        /**/

        if (flag_debug >= 3)
        {
            bool fname = false;
            ExpressionList* ops = code_line->operands; assert(ops);

            if (code_line->label)
            {
                Token* ltoken = code_line->label; assert(ltoken);
                FileString* ltext = ltoken->text; assert(ltext);

                message(ltext->file_name->c_str(), ":",
                        ltext->text_pos.line, ": label=\"",
                        ltext->c_str(), "\"");
                fname = true;
            }
            if (code_line->instr)
            {
                Token* itoken = code_line->instr; assert(itoken);
                FileString* itext = itoken->text; assert(itext);

                if (!fname)
                    message(itext->file_name->c_str(), ":",
                            itext->text_pos.line, ": ");
                else
                    message(", ");
                message("instr=\"", itext->c_str(), "\"");
            }
            message("\n");
            if (ops->size())
            {
                for (auto i : inc_range<size_t>(ops->size()))
                {
                    message("    operand[", i, "]:\n");
                    (*ops)[i]->dump(2);
                }
            }
        }

        /**/

        if (!code_line->instr)
        {
            assert(code_line->label);

            prepare_token();
            code_line->instr = Transfer(current_token);
            code_line->instab = it = instr_dummy;
        }
        if (it != eit)
        {
            auto& operation = it->second.operation;
            Instruction::PseudoCode pcode(operation);

            if (define_macro)
            {
                switch (pcode)
                {
                case Instruction::PSEUDO_ASSMBLE_SWITCH:
                case Instruction::PSEUDO_INCLUDE:
                case Instruction::PSEUDO_MESSAGE:
                case Instruction::PSEUDO_OPTION:
                case Instruction::PSEUDO_MACRO_BEGIN:
                case Instruction::PSEUDO_MACRO_END:
                case Instruction::PSEUDO_MACRO_DELETE:
                    break;
                default:
                    return parse_macro_append();
                }
            }
            (this->*pseudo_entry[pcode])();
            return true;
        }

        assert(mit != meit);
        if (define_macro)
            return parse_macro_append();
        return parse_macro();
    }

    bool Parser::parse_macro_append()
    {
        assert(define_macro);

        if (!token_line->size())
            return false;

        Token* et = &token_line->back(); assert(et);
        bool newline = (et->id == TokenID::EOL);

        if (!newline)
        {
            parse_warning(ErrorCode::NO_MACRO_END, {et});

            FileString* text = new FileString;
            Token* eol = new Token(TokenID::EOL, text);

            text->file_name = et->text->file_name;
            text->text_pos = et->text->text_pos;

            token_line->push_back(eol);
        }
        define_macro->code.push_back(token_line);
        if (!newline)
            define_macro = nullptr;
        return newline;
    }

    bool Parser::parse_macro()
    {
        CodeLinePtr line = code_line;
        Token* instr = line->instr; assert(instr);
        String* instr_text = instr->text; assert(instr_text);
        StdString& name = *instr_text;

        WASMGEN_DEBUG(2, "MACRO: start=\"", name ,"\"\n");

        if (macro_expand.find(name) != macro_expand.end())
            return parse_error(ErrorCode::NESTED_MACRO_EXPANSION, {instr});
        macro_expand.insert(name);

        MacroData* macro = line->macro; assert(macro);
        CodeLine* mline = macro->line; assert(mline);
        ExpressionList* mops = mline->operands; assert(mops);
        ExpressionList* ops = line->operands; assert(ops);

        if (!check_operands(line, mops->size(), mops->size()))
            return false;

        NewIdentifier idmap;

        for (auto n : inc_range<size_t>(mops->size()))
        {
            Expression* mop = (*mops)[n];
            Expression* op = (*ops)[n];
            String* name = mop->getname();

            if (!name)
                return parse_error(ErrorCode::NO_MACRO_ARGUMENT_NAME, {mop->token});
            (*idmap)[*name] = op;
        }

        assert(!token_stack.size());

        TokenStackPtr save_token_stack = alt_token_stack;
        auto& mcode = macro->code;

        alt_token_stack = new TokenStack;
        push_reader(macro_file);
        for (auto rl = mcode.rbegin(); rl != mcode.rend(); ++rl)
        {
            auto& cline = *rl;

            for (auto rc = cline->rbegin(); rc != cline->rend(); ++rc)
            {
                Token* token = *rc; assert(token);
                Expression* expr = idmap->getexpr(token->text);

                if (!expr)
                {
                    puttoken(token, true);
                    continue;
                }

                NewTokenList tlist;

                expr->gettokenlist(tlist);
                for (auto rt = tlist->rbegin(); rt != tlist->rend(); ++rt)
                    puttoken(*rt, true);
            }
        }
        while (parse_line())
            ;
        pop_reader();
        alt_token_stack = save_token_stack;

        if (error_count)
            parse_message(instr, " ここでマクロ \"", GetCStr(instr), "\" を展開しています。");
        macro_expand.erase(name);

        WASMGEN_DEBUG(2, "MACRO: end=\"", name,"\"\n");

        return !error_count;
    }

    /**/

    bool Parser::parse_operands()
    {
        ExpressionList* ops = code_line->operands; assert(ops);

        for (;;)
        {
            ExpressionRef expr(parse_expression());

            if (!expr)
                break;
            ops->push_back(Transfer(expr));

            TokenRef rt = next_token();

            if (Invalid(rt))
                return false;
            if (rt->id != TokenID::CHAR_COMMA)
            {
                puttoken(rt);
                break;
            }
        }

        ExpressionListRef nops = parse_expr_unpack(ops);

        if (!nops)
            return false;
        code_line->operands = Transfer(nops);
        return true;
    }

    ExpressionList *Parser::parse_expr_unpack(ExpressionList* list)
    {
        assert(list);

        NewExpressionList rlist;

        for (Expression* expr : *list)
        {
            ExpressionListRef children = parse_expr_unpack_list(expr);

            if (!children)
                rlist->push_back(expr);
            else
                rlist->insert(rlist->end(), children->begin(), children->end());
        }
        return Finish(rlist);
    }

    ExpressionList* Parser::parse_expr_unpack_list(Expression* expr)
    {
        assert(expr);

        switch (expr->mode)
        {
        case Expression::UNARY:
            break;
        case Expression::BINARY:
            return parse_expr_unpack_binary(expr);
        default:
            return nullptr;
        }

        Token* token = expr->token; assert(token);

        if (token->id != TokenID::MUL)
            return nullptr;

        ExpressionList* children = expr->children; assert(children && children->size() == 1);
        Expression* childexpr = (*children)[0]; assert(childexpr);
        Token* childtoken = childexpr->token; assert(childtoken);
        ExpressionList* grandchildren = childexpr->children; assert(grandchildren);

        switch (childexpr->mode)
        {
        case Expression::VALUE:
            if (childexpr->paren_open)
                return NewRef(children);
            {
                Identifier* idmap = alias_name; assert(idmap);
                Expression* symexpr = idmap->getexpr(childtoken->text);

                if (!symexpr)
                    return nullptr;
                if (symexpr->mode != Expression::LIST)
                    return nullptr;
                grandchildren = symexpr->children;
            }
            fallthrough;
        case Expression::LIST:
            return parse_expr_unpack(grandchildren);

        case Expression::BINARY:
            return parse_expr_unpack_binary(childexpr);

        default:
            break;
        }
        return nullptr;
    }

    ExpressionList *Parser::parse_expr_unpack_binary(Expression *expr)
    {
        assert(expr && expr->mode == Expression::BINARY);

        Token* token = expr->token; assert(token);
        ExpressionList* children = expr->children; assert(children);
        Expression* op0 = (*children)[0]; assert(op0);
        Expression* op1 = (*children)[1]; assert(op1);

        switch (token->id)
        {
        case TokenID::ADD:
            if (op0->mode != op1->mode)
                break;
            switch (op0->mode)
            {
            case Expression::VALUE:
                {
                    if (!op0->isstring() || !op1->isstring())
                        break;
                    token->id = TokenID::QUOTE;
                    token->quote = new String(*op0->getstring() + *op1->getstring());

                    NewExpressionList children;

                    children->push_back(new Expression(token));
                    return Finish(children);
                }

            case Expression::LIST:
                {
                    ExpressionList* op0_list = parse_expr_unpack(op0->children); assert(op0_list);
                    ExpressionList* op1_list = parse_expr_unpack(op1->children); assert(op1_list);

                    NewExpressionList children;

                    children->insert(children->end(), op0_list->begin(), op0_list->end());
                    children->insert(children->end(), op1_list->begin(), op1_list->end());
                    return Finish(children);
                }

            default:
                break;
            }
            break;

        case TokenID::MUL:
            if (op1->isnumber())
            {
                auto res = getvalue(op1);

                if (!res.isint())
                    break;
                if (op0->isstring())
                {
                    String* op0s = op0->getstring();

                    token->id = TokenID::QUOTE;
                    token->quote = new String;
                    for (int n = res, i = 0; i < n; ++i)
                        *token->quote += *op0s;

                    NewExpressionList children;

                    children->push_back(new Expression(token));
                    return Finish(children);
                }
                if (op0->mode == Expression::LIST)
                {
                    ExpressionList* op0_list = parse_expr_unpack(op0->children); assert(op0_list);
                    NewExpressionList children;

                    for (int n = res, i = 0; i < n; ++i)
                        children->insert(children->end(), op0_list->begin(), op0_list->end());
                    return Finish(children);
                }
            }
            break;

        case TokenID::VADD:
        case TokenID::VSUB:
        case TokenID::VMUL:
        case TokenID::VDIV:
        case TokenID::VMOD:
            if (op0->mode == Expression::LIST &&
                op1->mode == Expression::LIST)
            {
                ExpressionList* op0_list = parse_expr_unpack(op0->children); assert(op0_list);
                ExpressionList* op1_list = parse_expr_unpack(op1->children); assert(op1_list);
                size_t size = op0_list->size();

                if (size != op1_list->size())
                {
                    parse_error(ErrorCode::UNMATCHED_LIST_SIZES, {token});
                    break;
                }
                switch (token->id)
                {
                case TokenID::VADD: token->id = TokenID::ADD; break;
                case TokenID::VSUB: token->id = TokenID::SUB; break;
                case TokenID::VMUL: token->id = TokenID::MUL; break;
                case TokenID::VDIV: token->id = TokenID::DIV; break;
                case TokenID::VMOD: token->id = TokenID::MOD; break;
                default: break;
                }

                NewExpressionList children;

                for (auto n : inc_range<size_t>(size))
                    children->push_back(new Expression(token, (*op0_list)[n], (*op1_list)[n]));
                return Finish(children);
            }
            break;

        default:
            break;
        }
        return nullptr;
    }

    /*
     *
     */

    Expression* Parser::parse_expression()
    {
        if (!expr_nest)
            expr_error = false;

        ExprNest nest(expr_nest);

        return parse_expr_assignment();
    }

    Expression* Parser::parse_expr_assignment()
    {
        TokenRef sym = next_token();

        if (Invalid(sym))
            return nullptr;
        if (sym->id != TokenID::NAME)
        {
            puttoken(sym);
            return parse_expr_conditional();
        }

        TokenRef op = next_token();

        if (Invalid(op))
        {
            puttoken(sym);
            return parse_expr_conditional();
        }
        if (op->id != TokenID('='))
        {
            puttoken(op);
            puttoken(sym);
            return parse_expr_conditional();
        }

        ExpressionRef val = parse_expression();

        if (!val)
        {
            parse_expr_error(ErrorCode::SYNTAX_ERROR, {op});
            return nullptr;
        }

        return NewRef<Expression>(op, new Expression(sym), val);
    }

    Expression* Parser::parse_expr_conditional()
    {
        ExpressionRef op1, op2, op3;
        TokenRef sym1, sym2;

        if (!(op1 = parse_expr_binary()))
            return nullptr;
        sym1 = next_token();
        if (Invalid(sym1))
            return Finish(op1);
        if (*sym1 != TokenID('?'))
        {
            puttoken(sym1);
            return Finish(op1);
        }
        if (!(op2 = parse_expr_binary()))
        {
            parse_expr_error(ErrorCode::SYNTAX_ERROR, {sym1});
            return nullptr;
        }
        sym2 = next_token();
        if (Invalid(sym2) || *sym2 != TokenID(':'))
        {
            parse_expr_error(ErrorCode::SYNTAX_ERROR, {sym2 ? sym2 : op2->token});
            return nullptr;
        }
        if (!(op3 = parse_expr_binary()))
        {
            parse_expr_error(ErrorCode::SYNTAX_ERROR, {sym2});
            return nullptr;
        }

        if (op1->isnumber())
        {
            auto res = getvalue(op1); assert(res.isnumber());

            return Finish(res ? op2 : op3);
        }

        NewExpression expr(sym1, op1, op2, op3);
        NewTokenList xtlist;

        xtlist->push_back(sym1);
        xtlist->push_back(sym2);
        expr->token_list = Transfer(xtlist);
        return Finish(expr);
    }

    Expression* Parser::parse_expr_binary(int pop)
    {
        ExpressionRef lhs, rhs;
        TokenRef ct;

        if (!(lhs = parse_expr_unary()))
            return nullptr;
        for (;;)
        {
            ct = next_token();
            if (Invalid(ct))
                break;

            int nop = Expression::priority(ct->id);

            if (nop < 0 || (pop && (pop < nop)))
            {
                puttoken(ct);
                break;
            }
            if (!(rhs = parse_expr_binary(nop)))
            {
                parse_expr_error(ErrorCode::SYNTAX_ERROR, {lhs->token});
                return nullptr;
            }

            NewExpression expr(ct, lhs, rhs);

            if (expr_optimize && lhs->isnumber() && rhs->isnumber())
            {
                Token* vt = expr->gettoken(); assert(vt);
                auto res = getvalue(expr); assert(res.isnumber());
                ExpressionRef nexpr(res.isfloat()
                                    ? make_value(vt, res.fvalue)
                                    : make_value(vt, res.ivalue));

                expr = Transfer(nexpr);
            }

            lhs = Transfer(expr);
        }
        return Finish(lhs);
    }

    Expression* Parser::parse_expr_unary()
    {
        TokenRef rt = next_token();

        if (Invalid(rt))
            return nullptr;

        bool unary = Expression::is_unary_operator(rt->id);

        if (!unary)
        {
            puttoken(rt);
            return parse_expr_value();
        }

        ExpressionRef value = parse_expr_value();

        if (!value)
        {
            parse_expr_error(ErrorCode::SYNTAX_ERROR, {rt});
            return nullptr;
        }

        NewExpression expr(rt, value);

        if (!expr_optimize || !value->isnumber())
            return Finish(expr);

        Token* vt = expr->gettoken(); assert(vt);
        auto res = getvalue(expr);

        if (!res.isnumber())
            return nullptr;
        return res.isfloat()
            ? make_value(vt, res.fvalue)
            : make_value(vt, res.ivalue);
    }

    Expression* Parser::parse_expr_value()
    {
        ExpressionRef lhs, rhs;
        TokenRef ct;

        lhs = parse_expr_value_list();
        if (!lhs)
            return nullptr;
        for (;;)
        {
            ct = next_token();
            if (Invalid(ct))
                break;

            bool item = ct->id == TokenID('[');

            puttoken(ct);
            if (!item)
                break;
            if (!(rhs = parse_expr_value_list()))
                break;
            lhs = NewExpression(ct, lhs, rhs, Expression::ITEM);
        }
        return Finish(lhs);
    }

    Expression* Parser::parse_expr_value_list()
    {
        TokenRef rt = next_token();

        if (Invalid(rt))
            return nullptr;
        switch (rt->id)
        {
        case TokenID('('):
            {
                ExpressionRef value = parse_expr_list(TokenID(')'), rt);

                if (!value)
                    break;
                if (value->children->size() == 1 &&
                    (!value->token_list || !value->token_list->size()))
                {
                    auto& child = (*value->children)[0];

                    child->paren_open = value->paren_open;
                    child->paren_close = value->paren_close;
                    value = Transfer(child);
                }
                return Finish(value);
            }
            break;

        case TokenID('['):
            return parse_expr_list(TokenID(']'), rt);
        case TokenID('{'):
            return parse_expr_list(TokenID('}'), rt);

        case TokenID::NUMBER:
        case TokenID::NAME:
        case TokenID::QUOTE:
            return NewRef<Expression>(rt);

        default:
            puttoken(rt);
            break;
        }
        return nullptr;
    }

    Expression* Parser::parse_expr_list(TokenID cid, Token* st)
    {
        AutoRewind<decltype(skip_eol)>
            multiline(skip_eol, option.paren_multiline);

        NewExpression expr(Expression::LIST);
        NewTokenList xtlist;
        TokenPtr rt = st;
        Token* pt = nullptr;

        expr->token = st;
        expr->paren_open = st;
        for (;;)
        {
            pt = rt;
            rt = next_token();
            if (Invalid(rt))
            {
                parse_expr_error(ErrorCode::SYNTAX_ERROR, {pt});
                return nullptr;
            }
            if (rt->id == cid)
                break;
            puttoken(rt);

            ExpressionRef value = parse_expression();

            if (!value)
            {
                parse_expr_error(ErrorCode::SYNTAX_ERROR, {rt});
                return nullptr;
            }
            expr->children->push_back(Transfer(value));

            pt = rt;
            rt = next_token();
            if (Invalid(rt))
            {
                parse_expr_error(ErrorCode::SYNTAX_ERROR, {pt});
                return nullptr;
            }
            if (rt->id == cid)
                break;
            if (rt->id != TokenID::CHAR_COMMA)
            {
                parse_expr_error(ErrorCode::SYNTAX_ERROR, {pt});
                return nullptr;
            }
            xtlist->push_back(rt);
        }

        expr->token_list = Transfer(xtlist);
        expr->paren_close = rt;
        return Finish(expr);
    }

    /*
     *
     */

    void Parser::parse_code_line()
    {
        if (!code_list)
        {
            parse_invalid_line();
            return;
        }

        auto& instr = code_line->instr;
        auto& instab = code_line->instab;
        auto mit = instr_macro_dict.find(instab->first);

        if (mit == instr_macro_dict.end())
        {
            code_list->push_back(Transfer(code_line));
            return;
        }

        WASMGEN_DEBUG(2, "  expand: ", GetCStr(code_line->instr), "\n");

        ExpressionList* ops = code_line->operands; assert(ops);
        auto mcd = mit->second;

        assert(!code_list->pass);

        switch (mcd)
        {
        case Instruction::MC_COND_BR:
            if (!ops->size())
                break;
            {
                Expression* op0 = (*ops)[0]; assert(op0);
                bool op0_name = op0->isname();

                if (op0_name && *op0->getname() == *ins_return)
                {
                    NewCodeLine code_if;
                    NewCodeLine code_return;
                    NewCodeLine code_end;

                    code_line->operands = empty_operands;

                    code_if->instr = make_token(instr, *ins_if);
                    code_if->instab = instr_if;

                    code_return->instr = make_token(instr, *ins_return);
                    code_return->instab = instr_return;

                    code_end->instr = make_token(instr, *ins_end);
                    code_end->instab = instr_end;

                    code_list->push_back(Transfer(code_line));
                    code_list->push_back(Transfer(code_if));
                    code_list->push_back(Transfer(code_return));
                    code_list->push_back(Transfer(code_end));
                }
                else
                {
                    auto& ins_jcc = *ins_br_if;
                    NewCodeLine code_jcc(ops, 0);

                    code_line->operands = empty_operands;

                    code_jcc->label = Transfer(code_line->label);
                    code_jcc->instr = make_token(instr, ins_jcc);
                    code_jcc->instab = instr_br_if;

                    code_list->push_back(Transfer(code_line));
                    code_list->push_back(Transfer(code_jcc));
                }
            }
            return;

        default:
            throw BUG("未知のマクロ コード(", int(mcd), ")です。");
        }

        code_list->push_back(Transfer(code_line));
    }

    void Parser::parse_invalid_line()
    {
        parse_error(ErrorCode::INVALID_INSTRUCTION, {code_line->instr});
    }

    /*
     *
     */

    void Parser::parse_instruction()
    {
        (this->*instruction_entry)();
    }

    /*
     *
     */

    void Parser::parse_pseudo_assemble_switch(CodeLine* line)
    {
        WASMGEN_DEBUG(2, "Assemble Switch:\n");

        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_ASSEMBLE_SWITCH_LABEL, {label});

        auto& operation = line->instab->second.operation;
        ExpressionList* ops = line->operands; assert(ops);
        bool stack_push = true;

        switch (Instruction::AssembeSwitch(operation))
        {
        case Instruction::ASW_ELIF:
            if (!asmsw_stack.size())
            {
                parse_error(ErrorCode::NO_ASSEMBLY_SWITCH_IF, {line->instr});
                return;
            }
            if (asmsw_flag.set_else)
            {
                parse_error(ErrorCode::INVALID_ASSEMBLY_SWITCH_ELIF, {line->instr});
                return;
            }
            asmsw_flag.token = line->instr;
            if ((asmsw_flag.skip = asmsw_flag.skip_else))
                break;
            stack_push = false;
            fallthrough;
        case Instruction::ASW_IF:
            if (stack_push)
                asmsw_stack.push_back(asmsw_flag);
            asmsw_flag.token = line->instr;
            if (asmsw_flag.skip)
            {
                asmsw_flag.skip_if = true;
                asmsw_flag.skip_else = true;
                break;
            }
            switch (ops->size())
            {
            case 0:
                parse_error(ErrorCode::NO_ASSEMBLY_SWITCH_CONDITION, {line->instr});
                break;
            case 1:
                {
                    Expression* op = (*ops)[0]; assert(op);
                    auto res = getvalue(op, asmsw_name);

                    asmsw_flag.skip = !(res.isvalid() && bool(res));
                    asmsw_flag.skip_if = asmsw_flag.skip;
                    asmsw_flag.skip_else = !asmsw_flag.skip_if;
                }
                break;
            default:
                parse_error(ErrorCode::TOO_MANY_ASSEMBLY_SWITCH_CONDITION, {line->instr});
                break;
            }
            WASMGEN_DEBUG(2, "@if (skip=", asmsw_flag.skip, ")\n");
            break;

        case Instruction::ASW_ELSE:
            if (!asmsw_stack.size())
            {
                parse_error(ErrorCode::NO_ASSEMBLY_SWITCH_IF, {line->instr});
                return;
            }
            if (ops->size())
                parse_warning_too_many_operands(line, ops, 0);
            asmsw_flag.token = line->instr;
            asmsw_flag.skip = asmsw_flag.skip_else;
            asmsw_flag.set_else = true;

            WASMGEN_DEBUG(2, "@else (skip=", asmsw_flag.skip, ")\n");
            break;

        case Instruction::ASW_ENDIF:
            if (!asmsw_stack.size())
            {
                parse_error(ErrorCode::NO_ASSEMBLY_SWITCH_IF, {line->instr});
                return;
            }
            asmsw_flag = asmsw_stack.pop();

            WASMGEN_DEBUG(2, "@endif (nest=", asmsw_stack.size(), ")\n");
            break;

        case Instruction::ASW_SET:
            if (!ops->size())
            {
                parse_warning(ErrorCode::ASSEMBLE_SWITCH_NO_SET, {line->instr});
                return;
            }
            {
                Expression* op = (*ops)[0]; assert(op);
                GetEqualRes res;

                if (!geteqval(res, op, asmsw_name))
                    break;

                Token* tname = res.name; assert(tname);
                String* sname = tname->text; assert(sname);
                ExpressionRef opexr(res.value.isfloat()
                                    ? make_value(tname, res.value.fvalue)
                                    : make_value(tname, res.value.ivalue));

                (*define_name)[*sname] = Transfer(opexr);

                WASMGEN_DEBUG(2, "@set ", *sname, " = ", int(res.value), "\n");
            }
            break;

        case Instruction::ASW_UNSET:
            switch (ops->size())
            {
            default:
                parse_warning_too_many_operands(line, ops, 1);
                fallthrough;
            case 1:
                {
                    Expression* op_name = (*ops)[0]; assert(op_name);
                    Token* tname = op_name->token; assert(tname);

                    if (*tname != TokenID::NAME)
                    {
                        parse_error(ErrorCode::SYNTAX_ERROR, {tname});
                        break;
                    }

                    String* sname = tname->text; assert(sname);
                    auto it = define_name->find(*sname);

                    if (it != define_name->end())
                    {
                        define_name->erase(it);

                        WASMGEN_DEBUG(2, "@unset ", *sname, "\n");
                    }
                }
                break;
            case 0:
                parse_warning(ErrorCode::ASSEMBLE_SWITCH_NO_UNSET, {line->instr});
                break;
            }
            break;

        default:
            parse_invalid_line();
            break;
        }
    }

    void Parser::parse_pseudo_message()
    {
        auto& line = code_line;
        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_MESSAGE_LABEL, {label});

        ExpressionList* ops = line->operands; assert(ops);
        StdString msg;

        for (Expression* op : *ops)
        {
            assert(op);

            auto res = getvalue(op, asmsw_name);

            if (res.isvalid())
            {
                msg += res.str();
                continue;
            }

            String* str = getstring(op, asmsw_name);

            if (str)
            {
                msg += *str;
                continue;
            }

            WASMGEN_DEBUG(2, "Bug?!\n");
        }

        if (!msg.size() || msg[msg.size() - 1] != '\n')
            msg += '\n';

        auto& instr = line->instr;
        auto& text = instr->text;

        message(text->file_name->c_str(), ":", text->text_pos.line, ":message: ", msg.c_str());
    }

    void Parser::parse_pseudo_option()
    {
        auto& line = code_line;
        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_OPTION_LABEL, {label});

        ExpressionList* ops = line->operands; assert(ops);

        if (!ops->size())
        {
            parse_warning(ErrorCode::IGNORE_UNKNOWN_OPTION, {line->instr});
            return;
        }

        Expression* op = (*ops)[0]; assert(op);
        GetEqualRes res;

        if (!getequal(res, op, asmsw_name))
            return;

        Token* tname = res.name; assert(tname);
        String* name = tname->text; assert(name);

        if (!(false ||
              update_option(name, tname, res, op, optname_include_depth, option.include_depth, 0, 100) ||
              update_option(name, tname, res, op, optname_comment_nest, option.comment_nest) ||
              update_option(name, tname, res, op, optname_paren_multiline, option.paren_multiline) ||
              update_option(name, tname, res, op, optname_section_datacount, option.section_datacount) ||
              update_option(name, tname, res, op, optname_type_unique, option.type_unique) ||
              false))
            parse_warning(ErrorCode::IGNORE_UNKNOWN_OPTION, {tname});
        update_option();
    }

    void Parser::parse_pseudo_macro_begin()
    {
        WASMGEN_DEBUG(2, "DEFMACRO: start\n");

        auto& line = code_line;
        Token* label = line->label;

        if (define_macro)
        {
            parse_error(ErrorCode::NESTED_MACRO_DEFINITIONS, {label});
            return;
        }
        if (!label)
        {
            parse_error(ErrorCode::NO_MACRO_LABEL, {line->instr});
            return;
        }

        String* name = label->text; assert(name);

        if (macro_name->has(*name))
        {
            parse_error(ErrorCode::EXIST_MACRO_NAME, {label});
            return;
        }

        ExpressionList* ops = line->operands; assert(ops);

        (*macro_name)[*name] = new Expression(label, ops);

        define_macro = new MacroData(line);
        define_macro->line = line;

        macro_dict[*name] = define_macro;
    }

    void Parser::parse_pseudo_macro_end()
    {
        WASMGEN_DEBUG(2, "DEFMACRO: end\n");

        auto& line = code_line;
        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_CODE_LABEL, {label});
        check_operands(line, 0, 0);

        if (!define_macro)
            parse_warning(ErrorCode::NO_MACRO_DEFINITION, {line->instr});
        define_macro = nullptr;
    }

    void Parser::parse_pseudo_macro_delete()
    {
        WASMGEN_DEBUG(2, "DEFMACRO: delete\n");

        auto& line = code_line;
        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_CODE_LABEL, {label});
        if (!check_operands(line, 1, 2))
            return;

        if (define_macro)
        {
            parse_error(ErrorCode::CAN_NOT_DELETE_MACRO, {line->instr});
            return;
        }

        ExpressionList* ops = line->operands; assert(ops);

        Expression* op0 = (*ops)[0]; assert(op0);
        bool op0_name = op0->isname();

        if (!op0_name)
        {
            parse_error(ErrorCode::INVALID_MACRO_NAME, {line->instr});
            return;
        }

        String* name = op0->getname();
        auto dit = macro_dict.find(*name);
        auto nit = macro_name->find(*name);

        if (dit == macro_dict.end() || nit == macro_name->end())
        {
            assert(dit == macro_dict.end());
            assert(nit == macro_name->end());
            parse_warning(ErrorCode::MACRO_NOT_DEFINED, {*op0});
            return;
        }

        macro_dict.erase(dit);
        macro_name->erase(nit);
    }

    void Parser::parse_pseudo_alias()
    {
        auto& line = code_line;
        Token* label = line->label; assert(label);
        String* name = label->text; assert(name);

        if (global_name->has(*name))
        {
            parse_error(ErrorCode::EXIST_ALIAS_NAME, {label});
            return;
        }

        ExpressionList* ops = line->operands; assert(ops);

        switch (ops->size())
        {
        case 0:
            parse_error_too_few_operands(line);
            break;
        case 1:
            (*alias_name)[*name] = (*ops)[0];
            break;
        default:
            (*alias_name)[*name] = new Expression(label, ops);
            break;
        }
    }

    void Parser::parse_pseudo_include()
    {
        auto& line = code_line;
        Token* label = line->label;

        if (label)
            parse_warning(ErrorCode::IGNORE_INCLUDE_LABEL, {label});

        ExpressionList* ops = line->operands; assert(ops);
        Token* tname = nullptr;
        StringPtr fname;
        bool fexist = true;

        switch (ops->size())
        {
        default:
            parse_warning_too_many_operands(line, ops, 2);
            fallthrough;
        case 2:
            {
                Expression* op1 = (*ops)[1]; assert(op1);
                auto res = getvalue(op1, predefined_name);

                if (!res.isnumber())
                {
                    parse_error(ErrorCode::NOT_VALUE, {*op1});
                    return;
                }
                fexist = res;
            }
            fallthrough;
        case 1:
            {
                Expression* op0 = (*ops)[0]; assert(op0);
                auto res = getvalue(op0);

                tname = *op0;
                if (!res.isname() && !res.isstring())
                {
                    parse_error(ErrorCode::NOT_STRING, {tname});
                    return;
                }
                fname = Transfer(res.string);
            }
            break;

        case 0:
            parse_error(ErrorCode::NO_INCLUDE_FILE, {line->instr});
            return;
        }

        if (option.include_depth <= int(freader_stack.size()))
        {
            parse_error(ErrorCode::INCLUDE_NESTING_LIMIT, {line->instr});
            return;
        }
        {
            StringListRef dnf = FileString::dirandfile(current_directory, fname);
            NewTextFileReader freader(&(*dnf)[1]);

            if (freader->valid())
            {
                if (!parse_file((*dnf)[0], freader))
                    parse_message(tname, " ここからインクルードしています。");
                return;
            }
        }
        for (String* dir : *includes)
        {
            StringListRef dnf = FileString::dirandfile(dir, fname);
            NewTextFileReader freader(&(*dnf)[1]);

            if (freader->valid())
            {
                if (!parse_file((*dnf)[0], freader))
                    parse_message(tname, " ここからインクルードしています。");
                return;
            }
        }
        WASMGEN_DEBUG(2, "@include:error:[", int(fexist), "]: ", GetCStr(tname), "\n");

        if (fexist)
            parse_error(ErrorCode::NO_FILE, {tname});
        else if (flag_verbose)
        {
            StringBlock msg {"warning: @include ", GetCStr(tname), " の失敗を無視します。"};

            parse_message(tname, msg);
        }
    }

    /*
     *
     */

    void Parser::parse_pseudo_empty()
    {
        Section* sec = current_section;

        if (sec)
            switch (sec->id)
            {
            case SectionID::FUNCTION:
            case SectionID::GLOBAL:
            case SectionID::ELEMENT:
            case SectionID::CODE:
            case SectionID::DATA:
                parse_code_line();
                return;
            default:
                break;
            }
        parse_invalid_line();
    }

    void Parser::parse_pseudo_binary()
    {
        (this->*pseudo_data_entry)();
    }

    /*
     *
     */

    void Parser::parse_pseudo_custom()
    {
        next_section(SectionID::CUSTOM);
        pseudo_data_entry = &Parser::parse_code_line;

        code_list->binary = true;
    }

    void Parser::parse_pseudo_data()
    {
        next_section(SectionID::DATA);
        instruction_entry = &Parser::parse_code_line;
        pseudo_data_entry = &Parser::parse_code_line;
    }

    /**/

    void Parser::parse_pseudo_import_parameter()
    {
        auto sec = current_section;

        if (sec && sec->id == SectionID::IMPORT)
            parse_code_line();
        else
            parse_invalid_line();
    }

    void Parser::parse_pseudo_function_parameter()
    {
        auto sec = current_section;

        if (sec)
        {
            auto& instr = code_line->instab->second.operation;

            if (sec->id == SectionID::FUNCTION ||
                (sec->id == SectionID::TYPE &&
                 instr == Instruction::FPC_PARAM))
            {
                parse_code_line();
                return;
            }
        }
        parse_invalid_line();
    }

    void Parser::parse_pseudo_element_parameter()
    {
        auto sec = current_section;

        if (sec)
        {
            auto& instr = code_line->instab->second.operation;

            if (sec->id == SectionID::ELEMENT ||
                (sec->id == SectionID::DATA &&
                 instr == Instruction::EPC_OFFSET))
            {
                parse_code_line();
                return;
            }
        }
        parse_invalid_line();
    }

    void Parser::parse_pseudo_data_parameter()
    {
        auto sec = current_section;

        if (sec && sec->id == SectionID::DATA)
            parse_code_line();
        else
            parse_invalid_line();
    }

    /*
     *
     */

    bool Parser::parse_pseudo_function_parameter(GetNameValTypeRes& res, CodeLine* code)
    {
        assert(code);

        res = GetNameValTypeRes();
        if (!check_operands(code, 1, 1))
            return false;

        Token* label = code->label;
        String* name = GetText(label);
        GetTypeRes gvres;

        if (!getvaltype(gvres, (*code->operands)[0]))
            return false;
        res.name = name;
        res.valtype = gvres.valtype;
        return true;
    }

    /*
     *
     */

    bool Parser::parse_section()
    {
        for (Section* sec : *section_list)
            module->section[int(sec->id)]->push_back(sec);
        return (true &&
                parse_custom_section() &&
                parse_type_section() &&
                parse_import_section() &&
                parse_function_section() &&
                parse_table_section() &&
                parse_memory_section() &&
                parse_global_section() &&
                parse_export_section() &&
                parse_start_section() &&
                parse_element_section() &&
                parse_data_section() &&
                true);
    }

    /*
     *
     */

    bool Parser::parse_custom_section()
    {
        for (Section* sec : *module->section[int(SectionID::CUSTOM)])
            if (!parse_custom_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_custom_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Custom Section:\n");

        assert(sec);
        assert(sec->code);

        auto& code_list = *sec->code;

        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* line = *pcode++; assert(line);
        ExpressionList* ops = line->operands; assert(ops);

        assert(line->instab->second.operation == Instruction::PSEUDO_CUSTOM);

        String* name = nullptr;
        String* secname = nullptr;
        SectionID sid = SectionID::CUSTOM;

        switch (ops->size())
        {
        default:
            parse_warning_too_many_operands(line, ops, 2);
            fallthrough;
        case 2:
            {
                Expression* op1 = (*ops)[1]; assert(op1);

                if ((secname = getstring(op1)))
                {
                    int tid = section_name_dict.get(*secname, -1);

                    WASMGEN_DEBUG(2, "  section=\"", secname->c_str(), "\", id=", tid, "\n");

                    if (tid < 0)
                        return parse_error(ErrorCode::INVALID_SECTION_NAME, {*op1});
                    sid = SectionID(tid);
                }
                else
                {
                    auto vtmp = getvalue(op1);
                    int tid = vtmp;

                    if (!vtmp.isnumber())
                        return parse_error(ErrorCode::INVALID_SECTION_ID, {*op1});

                    WASMGEN_DEBUG(2, "  section id=", tid, "\n");

                    if (!in_range(0, tid, int(Section::size) - 1))
                        return parse_error(ErrorCode::INVALID_SECTION_ID, {*op1});

                    sid = SectionID(tid);
                }
            }
            fallthrough;
        case 1:
            {
                Expression* op0 = (*ops)[0]; assert(op0);

                name = getstring(op0);
                if (!name)
                    return parse_error(ErrorCode::NO_CUSTOM_SECTION_NAME, {*op0});

                WASMGEN_DEBUG(2, "  name=\"", name->c_str(), "\"\n");
            }
            break;

        case 0:
            return parse_error(ErrorCode::NO_CUSTOM_SECTION_NAME, {line->instr});
        }

        CustomSection* mod_custom = module->custom_section; assert(mod_custom);
        auto& psec = mod_custom->section[int(sid)];

        if (psec)
            return parse_error(ErrorCode::EXIST_CUSTOM_SECTION, {line->instr});

        Token* label = line->label;

        if (label)
        {
            parse_warning(ErrorCode::IGNORE_CUSTOM_SECTION_LABEL, {label});

            WASMGEN_DEBUG(2, "  label=\"", label->text->c_str(), "\"\n");
        }

        auto data = new CustomSectionData;

        data->id = sid;
        data->name = name;
        data->label = label;

        data->code = new CodeList(pcode, pend);
        data->code->binary = true;
        add_symbol(SectionID::CUSTOM, data->code);

        psec = data;
        return true;
    }

    /*
     *
     */

    bool Parser::parse_type_section()
    {
        for (Section* sec : *module->section[int(SectionID::TYPE)])
            if (!parse_type_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_type_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Type Section:\n");

        assert(sec);
        assert(sec->code);

        NewTypeData data;

        auto& code_list = *sec->code;
        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* line = *pcode++; assert(line);

        assert(line->instab->second.operation == Instruction::PSEUDO_TYPE);

        Token* label = line->label;
        String* name = nullptr;

        if (!label)
        {
            if (option.type_unique)
                return parse_error(ErrorCode::NO_SECTION_LABEL, {line->instr});
        }
        else
        {
            name = label->text; assert(name);

            WASMGEN_DEBUG(2, "  label=\"", name->c_str(), "\"\n");

            if (typeidx_name->has(*name))
                return parse_error(ErrorCode::EXIST_TYPE_NAME, {label});

            data->name = name;
        }

        ExpressionList* ops = line->operands; assert(ops);

        if (!getvaltypes(data->type.res, ops))
            return false;

        while (pcode != pend)
            if (!parse_type_section_param(data->type.arg, data->arg, *pcode++))
                return false;

        if (!parse_type_section_finish(label, data))
            return false;

        WASMGEN_DEBUG(2, "  typeidx=", data->index, "\n");
        return true;
    }

    bool Parser::parse_type_section_param(ResultType& restype, Identifier* idmap, CodeLine* line)
    {
        assert(idmap);
        assert(line);

        GetNameValTypeRes res;

        if (!parse_pseudo_function_parameter(res, line))
            return false;

        String* name = res.name;
        ValType valtype = res.valtype;

        if (!name)
            return parse_error(ErrorCode::NO_PARAM_LABEL, {line->instr});
        if (idmap->has(*name))
            return parse_error(ErrorCode::EXIST_PARAM_NAME, {line->label});
        if (valtype == ValType::NONE)
            return parse_error(ErrorCode::NOT_VALTYPE, {*line->getop(0)});
        if (name)
            append_index(idmap, line->label, restype.size());
        restype.push_back(valtype);

        WASMGEN_DEBUG(2,
                      "  param: ", line->getop(0)->getnamecstr(), "=", int(valtype),
                      ", name=\"", name ? name->c_str() : "(null)", "\"\n");
        return true;
    }

    bool Parser::parse_type_section_finish(Token* label, TypeData* data)
    {
        TypeSection* mod_type = module->type_section; assert(mod_type);
        size_t index = add_functype(data->type);

        data->index = int(index);
        mod_type->push_back(data);

        if (label)
        {
            String* name = label->text; assert(name);

            mod_type->name[*name] = data;
            append_index(module->typeidx, label, index);
        }
        return true;
    }

    /*
     *
     */

    bool Parser::parse_import_section()
    {
        for (Section* sec : *module->section[int(SectionID::IMPORT)])
            if (!parse_import_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_import_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Import Section:\n");

        assert(sec);
        assert(sec->code);

        for (CodeLine* line : *sec->code)
        {
            auto& pins= line->instab->second;
            auto& instr = pins.operation;

            switch (Instruction::PseudoCode(instr))
            {
            case Instruction::PSEUDO_IMPORT:
                if (Instruction::ImportCode(instr) == Instruction::IMP_NONE)
                {
                    if (!parse_import_section_module(line))
                        return false;
                    break;
                }
                fallthrough;
            case Instruction::PSEUDO_IMPORT_PARAM:
                if (!parse_import_section_param(line))
                    return false;
                break;
            default:
                parse_bug_unknown_pseudo_code(line);
            }
        }
        return true;
    }

    bool Parser::parse_import_section_module(CodeLine* line)
    {
        if (!check_operands(line, 1, 1))
            return false;

        ExpressionList* ops = line->operands; assert(ops);
        Expression* op = (*ops)[0]; assert(op);
        String* mod = getstring(op);

        if (!mod)
            return parse_error(ErrorCode::NO_MODULE_NAME, {line->instr});
        current_module_name = mod;

        WASMGEN_DEBUG(2, "  mod=\"", mod->c_str(), "\"\n");
        return true;
    }

    bool Parser::parse_import_section_param(CodeLine* line)
    {
        NewImportData impdat;

        Token* label = line->label;
        String* impname = GetText(label);

        if (impname)
        {
            impdat->label = impname;

            WASMGEN_DEBUG(2, "  label=\"", impname->c_str(), "\"\n");
        }

        auto& pins= line->instab->second;
        auto& instr = pins.operation;

        ExpressionList* ops = line->operands; assert(ops);
        auto opp = ops->begin();
        auto ope = ops->end();
        String* mod = nullptr;

        switch (Instruction::PseudoCode(instr))
        {
        case Instruction::PSEUDO_IMPORT:
            {
                Expression* op_mod = *opp++;

                if (!(mod = getstring(op_mod)))
                    return parse_error(ErrorCode::NO_IMPORT_NAME, {*op_mod});
                impdat->mod = mod;
            }
            break;

        case Instruction::PSEUDO_IMPORT_PARAM:
            if (!(mod = current_module_name))
                return parse_error(ErrorCode::NO_MODULE_NAME, {line->instr});
            impdat->mod = mod;
            break;

        default:
            parse_bug_unknown_pseudo_code(line);
        }
        WASMGEN_DEBUG(2, "  mod=\"", mod->c_str(), "\"\n");

        if (opp == ope)
            return parse_error_too_few_operands(line);

        Expression* op_name = *opp++;
        String* name = getstring(op_name);

        if (!name)
            return parse_error(ErrorCode::NO_IMPORT_NAME, {*op_name});
        impdat->name = name;

        WASMGEN_DEBUG(2, "  name=\"", name->c_str(), "\"\n");

        if (opp == ope)
            return parse_error_too_few_operands(line);

        int mut = 0;
        bool minmax = false;

        switch (Instruction::ImportCode(instr))
        {
        case Instruction::IMP_FUNC:
            if (impname && funcidx_name->has(*impname))
                return parse_error(ErrorCode::EXIST_FUNCTION_NAME, {label});
            {
                Expression* op_type = *opp++; assert(op_type);
                int typeidx = -1;

                if (op_type->mode == Expression::LIST)
                {
                    NewTypeData data;

                    if (impname)
                    {
                        if (typeidx_name->has(*impname))
                            return parse_error(ErrorCode::EXIST_TYPE_NAME, {label});
                        data->name = impname;
                    }
                    if (!getfunctype(data->type, op_type))
                        return false;
                    if (!parse_type_section_finish(label, data))
                        return false;
                    typeidx = data->index;
                }
                else
                {
                    auto vtmp = getvalue(op_type, typeidx_name);

                    if (!vtmp.isnumber())
                        return parse_error(ErrorCode::NO_TYPE_NAME, {*op_type});
                    typeidx = vtmp;
                }

                impdat->desc = ImportData::FUNCTION;
                impdat->typeidx = typeidx;

                WASMGEN_DEBUG(2, "  desc=", int(impdat->desc), "\n");
                WASMGEN_DEBUG(2, "  typeidx=", impdat->typeidx, "\n");
            }
            break;

        case Instruction::IMP_TABLE:
            if (impname && tableidx_name->has(*impname))
                return parse_error(ErrorCode::EXIST_TABLE_NAME, {label});
            {
                Expression* op_reftype = *opp++; assert(op_reftype);
                GetTypeRes res;

                if (!getreftype(res, op_reftype))
                    return false;
                impdat->desc = ImportData::TABLE;
                impdat->valtype = res.valtype;

                minmax = true;

                WASMGEN_DEBUG(2, "  desc=", int(impdat->desc), "\n");
                WASMGEN_DEBUG(2, "  valtype: ", op_reftype->getstrcstr(), "=", int(impdat->valtype), "\n");
            }
            break;

        case Instruction::IMP_MEMORY:
            if (impname && memidx_name->has(*impname))
                return parse_error(ErrorCode::EXIST_MEMORY_NAME, {label});
            {
                impdat->desc = ImportData::MEMORY;

                minmax = true;

                WASMGEN_DEBUG(2, "  desc=", int(impdat->desc), "\n");
            }
            break;

        case Instruction::IMP_VAR:
            mut = 1;
            fallthrough;
        case Instruction::IMP_CONST:
            if (impname && globalidx_name->has(*impname))
                return parse_error(ErrorCode::EXIST_GLOBAL_NAME, {label});
            {
                Expression* op_valtype = *opp++; assert(op_valtype);
                GetTypeRes res;

                if (!getvaltype(res, op_valtype))
                    return false;
                impdat->desc = ImportData::GLOBAL;
                impdat->valtype = res.valtype;
                impdat->mut = mut;

                WASMGEN_DEBUG(2, "  desc=", int(impdat->desc), "\n");
                WASMGEN_DEBUG(2, "  valtype: ", op_valtype->getstrcstr(), "=", int(impdat->valtype), "\n");
                WASMGEN_DEBUG(2, "  mut=", mut, "\n");
            }
            break;

        default:
            parse_bug_unknown_pseudo_code(line);
        }
        if (minmax)
        {
            if (opp == ope)
                return parse_error_too_few_operands(line);
            if (!getlimits(impdat->limits, line, opp - ops->begin()))
                return false;
            opp += impdat->limits.mode + 1;

            WASMGEN_DEBUG(2, "  limits: ", impdat->limits.dump(), "\n");
        }

        if (impname)
            switch (Instruction::ImportCode(instr))
            {
            case Instruction::IMP_FUNC:
                impdat->index = int(append_index(module->funcidx, label));
                break;

            case Instruction::IMP_TABLE:
                impdat->index = int(append_index(module->tableidx, label));
                break;

            case Instruction::IMP_MEMORY:
                impdat->index = int(append_index(module->memidx, label));
                break;

            case Instruction::IMP_VAR:
            case Instruction::IMP_CONST:
                impdat->index = int(append_index(module->globalidx, label));
                break;

            default:
                break;
            }

        if (opp != ope)
            parse_warning_too_many_operands(line, **opp, opp - ops->begin());

        WASMGEN_DEBUG(2, "  index=", impdat->index, "\n");

        module->import_section->push_back(Transfer(impdat));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_function_section()
    {
        for (Section* sec : *module->section[int(SectionID::FUNCTION)])
            if (!parse_function_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_function_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Function/Code Section:\n");

        assert(sec);
        assert(sec->code);

        auto& code_list = *sec->code;
        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* sline = *pcode++; assert(sline);
        Token* label = sline->label;
        String* name = GetText(label);

        assert(sline->instab->second.operation == Instruction::PSEUDO_FUNCODE);

        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {sline->instr});
        if (funcidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_FUNCTION_NAME, {label});

        ExpressionList* ops = sline->operands; assert(ops);
        Expression* op0 = ops->size() > 0 ? (*ops)[0] : nullptr;
        TypeDataPtr type = op0 ? gettypedata(op0) : nullptr;
        bool ftype = false;

        if (op0 && type)
        {
            if (ops->size() > 1)
                parse_warning_too_many_operands(sline, ops, 1);

            WASMGEN_DEBUG(2, "  type=\"", op0->getnamecstr(), "\"\n");
        }
        else
        {
            if (typeidx_name->has(*name))
                return parse_error(ErrorCode::EXIST_TYPE_NAME, {label});

            ftype = true;
            type = new TypeData;
            type->name = name;

            if (!getvaltypes(type->type.res, ops))
                return false;
        }

        NewFunctionCode func;
        CodeLineList params;
        CodeLineList locals;

        func->name = name;
        func->type = type;

        while (pcode != pend)
        {
            auto pline = *pcode++;
            auto& operation = pline->instab->second.operation;

            if (operation != Instruction::PSEUDO_FUNC_PARAM)
                func->code->push_back(pline);
            else if (operation == Instruction::FPC_PARAM)
                params.push_back(pline);
            else if (operation == Instruction::FPC_LOCAL)
                locals.push_back(pline);
            else
                parse_bug_unknown_pseudo_code(pline);
        }

        if (ftype)
        {
            for (auto pline : params)
                if (!parse_type_section_param(type->type.arg, type->arg, pline))
                    return false;
            if (typeidx_name->has(*name))
                return parse_error(ErrorCode::EXIST_TYPE_NAME, {label});
            if (!parse_type_section_finish(label, type))
                return false;
        }
        else if (params.size())
            return parse_error(ErrorCode::FUNCTION_FIXED_PARAMETERS, {sline->instr});

        if (!parse_function_section_local(func, locals))
            return false;

        func->index = int(append_index(module->funcidx, label));

        WASMGEN_DEBUG(2, "  funcidx=", func->index, "\n");

        FunctionSection* mod_function = module->function_section; assert(mod_function);

        mod_function->name[*name] = func;
        mod_function->push_back(Transfer(func));
        return true;
    }

    bool Parser::parse_function_section_local(FunctionCode* func, CodeLineList& locals)
    {
        using tokenlist = StdVector<Token*>;
        using valtypemap = StdMap<ValType, tokenlist>;

        assert(func);

        StdVector<CodeLine*> pending;
        TypeData* type = func->type; assert(type);
        Identifier* args = &type->arg;
        valtypemap vtmap;

        for (CodeLine* line : locals)
        {
            GetNameValTypeRes res;

            if (!parse_pseudo_function_parameter(res, line))
                return false;

            Token* label = line->label;
            String* name = res.name;
            ValType valtype = res.valtype;

            if (!name)
                return parse_error(ErrorCode::NO_PARAM_LABEL, {line->instr});
            if (valtype == ValType::NONE)
            {
                pending.push_back(line);
                continue;
            }
            vtmap[valtype].push_back(label);
        }

        StdVector<ValType> vtlist;

        for (auto item : vtmap)
            vtlist.push_back(item.first);
        std::sort(vtlist.begin(), vtlist.end());
        std::reverse(vtlist.begin(), vtlist.end());

        NewIdentifier lid(args);

        for (auto vt : vtlist)
        {
            auto& labels = vtmap[vt];

            WASMGEN_DEBUG(2, "  local: valtype=", int(vt), "\n");

            func->locals.push_back({ labels.size(), vt });
            for (auto label : labels)
            {
                size_t index = append_index(lid, label);

                WASMGEN_DEBUG(2, "    name[", index, "]=\"", GetCStr(label), "\"\n");
            }
        }

        for (CodeLine* line : pending)
        {
            assert(line);

            Token* label = line->label; assert(label);
            ExpressionList* ops = line->operands; assert(ops); assert(ops->size() >= 1);
            Expression* op = (*ops)[0]; assert(op);
            Token* op_token = op->token; assert(op_token);
            String* name = op_token->text; assert(name);
            Expression* expr = lid->getexpr(*name);

            if (!expr)
                return parse_error(ErrorCode::NOT_VALTYPE, {op_token});
            (*lid)[label->text] = expr;
        }

        CodeList* clist = func->code; assert(clist);

        func->local = lid;
        clist->label = lid;
        add_symbol(SectionID::FUNCTION, clist);
        return true;
    }

    /*
     *
     */

    bool Parser::parse_table_section()
    {
        for (Section* sec : *module->section[int(SectionID::TABLE)])
            if (!parse_table_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_table_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Table Section:\n");

        assert(sec);
        assert(sec->code);
        assert(sec->code->size() == 1);

        CodeLine* line = sec->code->front();
        NewTableData tab;

        assert(line);
        if (!check_operands(line, 2, 3))
            return false;

        Token* label = line->label; assert(label);
        String* name = GetText(label);

        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {line->instr});
        if (tableidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_TABLE_NAME, {label});
        tab->name = name;

        ExpressionList* ops = line->operands; assert(ops);
        Expression* op = (*ops)[0]; assert(op);
        GetTypeRes res;

        if (!getreftype(res, op))
            return false;
        tab->reftype = res.valtype;
        if (!getlimits(tab->limits, line, 1))
            return false;

        tab->index = int(append_index(module->tableidx, label));

        WASMGEN_DEBUG(2, "  valtype: ", op->getstrcstr(), "=", int(tab->reftype), "\n");
        WASMGEN_DEBUG(2, "  limits: ", tab->limits.dump(), "\n");
        WASMGEN_DEBUG(2, "  tableidx=", tab->index, "\n");

        module->table_section->push_back(Transfer(tab));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_memory_section()
    {
        for (Section* sec : *module->section[int(SectionID::MEMORY)])
            if (!parse_memory_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_memory_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Memory Section:\n");

        assert(sec);
        assert(sec->code);
        assert(sec->code->size() == 1);

        CodeLine* line = sec->code->front(); assert(line);

        NewMemoryData mem;

        assert(line);
        if (!check_operands(line, 1, 2))
            return false;

        Token* label = line->label;
        String* name = GetText(label);

        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {line->instr});
        if (memidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_MEMORY_NAME, {label});
        mem->name = name;

        if (!getlimits(mem->limits, line, 0))
            return false;

        mem->index = int(append_index(module->memidx, label));

        WASMGEN_DEBUG(2, "  limits: ", mem->limits.dump(), "\n");
        WASMGEN_DEBUG(2, "  memidx=", mem->index, "\n");

        module->memory_section->push_back(Transfer(mem));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_global_section()
    {
        for (Section* sec : *module->section[int(SectionID::GLOBAL)])
            if (!parse_global_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_global_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Global Section:\n");

        assert(sec);
        assert(sec->code);

        auto& code_list = *sec->code;
        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* sline = *pcode++; assert(sline);
        auto& instab = sline->instab->second;
        bool mut = (instab.operation == Instruction::PG_VAR);
        Token* label = sline->label;
        String* name = GetText(label);

        assert(instab.operation == Instruction::PSEUDO_GLOBAL);

        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {sline->instr});
        if (globalidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_GLOBAL_NAME, {label});

        WASMGEN_DEBUG(2, "  label=\"", name->c_str(), "\"\n");

        ExpressionList* ops = sline->operands; assert(ops);

        if (ops->size() < 1)
            return parse_error_too_few_operands(sline);

        Expression* op0 = (*ops)[0]; assert(op0);
        auto gtres = gettype(valtype_dict, op0);
        ValType valtype = gtres.valtype;

        if (valtype == ValType::NONE)
            return parse_error(ErrorCode::NOT_VALTYPE, {*op0});

        WASMGEN_DEBUG(2, "  valtype: ", op0->getstrcstr(), "=", int(valtype), "\n");

        NewCodeList expr(pcode, pend);

        if (ops->size() > 1)
        {
            if (pcode != pend)
                return parse_error(ErrorCode::EXIST_GLOBAL_EXPR, {(*pcode)->instr});

            auto instr = StdString(*op0->getname() + ".const");
            auto instab = Instruction::table.find(instr);

            if (!make_expr(expr, sline->instr, instr, instab, 1, ops->begin() + 1, ops->end()))
                return parse_error(ErrorCode::CAN_NOT_BE_INLINED, {*op0});
        }

        NewGlobalData data;

        data->name = name;
        data->mut = mut;
        data->valtype = valtype;
        data->code = Transfer(expr);
        data->index = int(append_index(module->globalidx, label));

        add_symbol(SectionID::GLOBAL, data->code);

        WASMGEN_DEBUG(2, "  mut=", mut, "\n");
        WASMGEN_DEBUG(2, "  globalidx=", data->index, "\n");

        module->global_section->push_back(Transfer(data));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_export_section()
    {
        for (Section* sec : *module->section[int(SectionID::EXPORT)])
            if (!parse_export_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_export_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Export Section:\n");

        assert(sec);
        assert(sec->code);
        assert(sec->code->size() == 1);

        CodeLine* line = sec->code->front(); assert(line);
        auto& instab = line->instab;
        auto& ins = instab->second;
        auto& instr = ins.operation;
        Token* label = line->label;

        if (label)
        {
            parse_warning(ErrorCode::IGNORE_EXPORT_SECTION_LABEL, {label});

            WASMGEN_DEBUG(2, "  label=\"", GetCStr(label), "\"\n");
        }
        if (!check_operands(line, 1, 2))
            return false;

        ExpressionList* ops = line->operands; assert(ops);
        Expression* op0 = (*ops)[0]; assert(op0);
        Token* xtoken = op0->gettoken(); assert(xtoken);
        String* name = GetText(xtoken);
        ExprValue res;

        NewExportData data;
        bool desc = false;

        switch (Instruction::PseudoCode(instr))
        {
        case Instruction::PSEUDO_EXPORT:
            if ((desc = (res = getvalue(op0, globalidx_name)).isnumber()))
                data->desc = ExportData::GLOBAL;
            else if ((desc = (res = getvalue(op0, funcidx_name)).isnumber()))
                data->desc = ExportData::FUNCTION;
            else if ((desc = (res = getvalue(op0, tableidx_name)).isnumber()))
                data->desc = ExportData::TABLE;
            else if ((desc = (res = getvalue(op0, memidx_name)).isnumber()))
                data->desc = ExportData::MEMORY;
            break;

        case Instruction::PSEUDO_EXPORT_FUNC:
            if ((desc = (res = getvalue(op0, funcidx_name)).isnumber()))
                data->desc = ExportData::FUNCTION;
            break;

        case Instruction::PSEUDO_EXPORT_TABLE:
            if ((desc = (res = getvalue(op0, tableidx_name)).isnumber()))
                data->desc = ExportData::TABLE;
            break;

        case Instruction::PSEUDO_EXPORT_MEMORY:
            if ((desc = (res = getvalue(op0, memidx_name)).isnumber()))
                data->desc = ExportData::MEMORY;
            break;

        case Instruction::PSEUDO_EXPORT_GLOBAL:
            if ((desc = (res = getvalue(op0, globalidx_name)).isnumber()))
                data->desc = ExportData::GLOBAL;
            break;

        default:
            parse_bug_unknown_pseudo_code(line);
        }
        if (!desc)
            return parse_error(ErrorCode::NO_EXPORT_SYMBOL, {*op0});
        data->index = res;

        if (ops->size() > 1)
        {
            Expression* op1 = (*ops)[1];

            name = getstring(op1);
        }
        if (!name)
            return parse_error(ErrorCode::NO_EXPORT_NAME, {line->instr});
        data->name = name;

        WASMGEN_DEBUG(2, "  desc=", data->desc, "\n");
        WASMGEN_DEBUG(2, "  name=\"", data->name->c_str(), "\"\n");
        WASMGEN_DEBUG(2, "  index=", data->index, "\n");

        module->export_section->push_back(Transfer(data));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_start_section()
    {
        int index = -1;

        for (Section* sec : *module->section[int(SectionID::START)])
        {
            WASMGEN_DEBUG(2, "Start Section:\n");

            assert(sec);
            assert(sec->code);
            assert(sec->code->size() == 1);

            CodeLine* line = sec->code->front(); assert(line);

            if (!check_operands(line, 1, 1))
                return false;

            ExpressionList *ops = line->operands; assert(ops);
            Expression* op = (*ops)[0]; assert(op);
            auto res = getvalue(op, funcidx_name);

            if (!res.isvalid())
                return parse_error(ErrorCode::NO_FUNCTION_NAME, {*op});

            int fidx = int(res);

            if (index >= 0 && index != fidx)
                return parse_error(ErrorCode::TOO_MANY_STARTS, {line->instr});
            index = fidx;

            WASMGEN_DEBUG(2, "  funcidx=", index, "\n");
        }

        module->start_section->funcidx = index;
        return true;
    }

    /*
     *
     */

    bool Parser::parse_element_section()
    {
        for (Section* sec : *module->section[int(SectionID::ELEMENT)])
            if (!parse_element_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_element_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Element Section:\n");

        assert(sec);
        assert(sec->code);

        NewElementData elem;

        auto& code_list = *sec->code;
        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* sline = *pcode++; assert(sline);
        Token* label = sline->label;
        String* name = GetText(label);

        assert(sline->instab->second.operation == Instruction::PSEUDO_ELEMENT);
        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {sline->instr});
        if (elemidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_ELEMENT_NAME, {label});
        elem->name = name;

        WASMGEN_DEBUG(2, "  label=\"", name->c_str(), "\"\n");

        ExpressionList* sops = sline->operands; assert(sops);
        RefType reftype = RefType::NONE;
        ElementMode mode = ElementMode::NONE;
        bool set_tableidx = false;
        int tableidx = 0;

        auto opp = sops->begin();
        auto ope = sops->end();

        while (opp != ope)
        {
            Expression* op_reftype = *opp; assert(op_reftype);
            auto rt_res = gettype(reftype_dict, op_reftype);

            reftype = rt_res.valtype;
            if (reftype != RefType::NONE)
            {
                if (++opp == ope)
                    break;

                WASMGEN_DEBUG(2, "  reftype: ", op_reftype->getnamecstr(), "=", int(reftype), "\n");
            }

            Expression* op_mode = *opp; assert(op_mode);
            auto smode = getstring(op_mode);

            if (!smode)
                return parse_error(ErrorCode::NOT_ELEMENT_MDOE, {*op_mode});
            mode = element_mode_dict.get(*smode, ElementMode::NONE);
            if (mode == ElementMode::NONE)
                return parse_error(ErrorCode::NOT_ELEMENT_MDOE, {*op_mode});

            WASMGEN_DEBUG(2, "  mode=\"", op_mode->getnamecstr(), "\"\n");

            if (++opp == ope)
                break;
            if (mode != ElementMode::ACTIVE)
                break;

            Expression* op_tabidx = *opp;
            auto ti_res = getvalue(op_tabidx, tableidx_name);

            if (!ti_res.isnumber())
                return parse_error(ErrorCode::NO_TABLE_NAME, {*op_tabidx});

            int ntableidx(ti_res);

            if (ntableidx < 0)
                return parse_error(ErrorCode::INVALID_TABLEIDX, {*op_tabidx});

            set_tableidx = true;
            tableidx = ntableidx;

            WASMGEN_DEBUG(2, "  tableidx=", tableidx, "\n");

            if (++opp == ope)
                break;

            elem->offset_expr = new CodeList;
            if (!make_expr(elem->offset_expr, sline->instr, ValType::I32, int(opp - sops->begin()), opp, ope))
                return parse_error(ErrorCode::CAN_NOT_BE_INLINED, {**opp});
            add_symbol(SectionID::ELEMENT, elem->offset_expr);

            WASMGEN_DEBUG(2, "  offset=", (*opp)->gettokencstr(), "\n");

            opp = ope;
            break;
        }
        if (opp != ope)
            parse_warning_too_many_operands(sline, **opp, opp - sops->begin());

        /**/

        Instruction::ElemParamCode cepc = Instruction::EPC_NONE;
        bool funcidx_expr = false;
        NewElementInit init;

        while (pcode != pend)
        {
            CodeLine* line = *pcode++; assert(line);
            auto& instab = line->instab;
            auto& insparam = instab->second;
            auto& operation = insparam.operation;

            if (operation != Instruction::PSEUDO_ELEM_PARAM)
            {
                switch (cepc)
                {
                case Instruction::EPC_OFFSET:
                    assert(elem->offset_expr);
                    elem->offset_expr->push_back(line);
                    if (instab == instr_end)
                        cepc = Instruction::EPC_FUNCIDX;
                    continue;

                case Instruction::EPC_FUNCIDX:
                    if (operation != Instruction::INSTRUCTION)
                        break;
                    if (!init->code)
                    {
                        init->code = new CodeList;
                        add_symbol(SectionID::ELEMENT, init->code);
                    }
                    init->code->push_back(line);
                    funcidx_expr = true;
                    continue;

                default:
                    break;
                }
                return parse_error(ErrorCode::INVALID_ELEMENT_CODE, {line->instr});
            }

            auto nepc = Instruction::ElemParamCode(operation);

            switch (nepc)
            {
            case Instruction::EPC_REFTYPE:
                {
                    if (!check_operands(line, 1, 1))
                        return false;

                    Expression* op0 = (*line->operands)[0]; assert(op0);
                    auto rt_res = gettype(reftype_dict, op0);
                    auto rt_tmp = rt_res.valtype;

                    if (rt_tmp != RefType::FUNCREF && rt_tmp != RefType::EXTERNREF)
                        return parse_error(ErrorCode::NOT_REFTYPE, {*op0});
                    if (reftype != RefType::NONE && reftype != rt_tmp)
                        return parse_error(ErrorCode::TOO_MANY_REFTYPE, {*op0});
                    reftype = rt_tmp;

                    WASMGEN_DEBUG(2, "  reftype: ", op0->getnamecstr(), "=", int(reftype), "\n");
                }
                break;

            case Instruction::EPC_TABLEIDX:
                {
                    if (mode == ElementMode::NONE)
                        mode = ElementMode::ACTIVE;
                    if (mode != ElementMode::ACTIVE)
                        return parse_error(ErrorCode::NOT_ELEMENT_ACTIVE, {line->instr});
                    if (!check_operands(line, 1, 1))
                        return false;

                    Expression* op0 = (*line->operands)[0]; assert(op0);
                    auto ti_res = getvalue(op0, tableidx_name);

                    if (!ti_res.isnumber())
                        return parse_error(ErrorCode::NO_TABLE_NAME, {*op0});

                    int ntableidx(ti_res);

                    if (set_tableidx && tableidx != ntableidx)
                        return parse_error(ErrorCode::TOO_MANY_TABLEIDX, {*op0});
                    if (ntableidx < 0)
                        return parse_error(ErrorCode::INVALID_TABLEIDX, {*op0});
                    set_tableidx = true;
                    tableidx = ntableidx;

                    WASMGEN_DEBUG(2, "  tableidx=", tableidx, "\n");
                }
                break;

            case Instruction::EPC_OFFSET:
                {
                    if (mode == ElementMode::NONE)
                        mode = ElementMode::ACTIVE;
                    if (mode != ElementMode::ACTIVE)
                        return parse_error(ErrorCode::NOT_ELEMENT_ACTIVE, {line->instr});

                    if (elem->offset_expr)
                        return parse_error(ErrorCode::TOO_MANY_OFFSET, {line->instr});
                    elem->offset_expr = new CodeList;
                    add_symbol(SectionID::ELEMENT, elem->offset_expr);

                    ExpressionList* ops = line->operands; assert(ops);

                    cepc = nepc;
                    if (ops->size())
                    {
                        if (!make_expr(elem->offset_expr, sline->instr, ValType::I32, 0, ops->begin(), ops->end()))
                            return parse_error(ErrorCode::CAN_NOT_BE_INLINED, {*(*ops)[0]});
                        cepc = Instruction::EPC_FUNCIDX;
                    }
                }
                break;

            case Instruction::EPC_FUNCIDX:
                {
                    ExpressionList* ops = line->operands; assert(ops);

                    if (init->code)
                    {
                        elem->init->push_back(Transfer(init));
                        init = new ElementInit;
                    }

                    for (Expression* op : *ops)
                    {
                        assert(op);

                        auto res = getvalue(op, funcidx_name);

                        if (!res.isnumber())
                            return parse_error(ErrorCode::NO_FUNCTION_NAME, {*op});

                        int funcidx = res;

                        init->index.push_back(funcidx);

                        WASMGEN_DEBUG(2, "  funcidx=", funcidx, "\n");
                    }

                    cepc = nepc;
                }
                break;

            default:
                parse_bug_unknown_pseudo_code(line);
            }
        }
        if (!init->empty())
            elem->init->push_back(Transfer(init));

        if (reftype == RefType::NONE)
            reftype = RefType::FUNCREF;
        if (mode == ElementMode::NONE)
            mode = ElementMode::ACTIVE;

        elem->reftype = reftype;
        elem->mode = mode;
        elem->tableidx = tableidx;
        elem->funcidx_expr = funcidx_expr;
        elem->index = int(append_index(module->elemidx, label));

        module->element_section->push_back(Transfer(elem));
        return true;
    }

    /*
     *
     */

    bool Parser::parse_data_section()
    {
        for (Section* sec : *module->section[int(SectionID::DATA)])
            if (!parse_data_section(sec))
                return false;
        return !error_count;
    }

    bool Parser::parse_data_section(Section* sec)
    {
        WASMGEN_DEBUG(2, "Data Section:\n");

        assert(sec);
        assert(sec->code);

        NewDataBlock data;

        auto& code_list = *sec->code;
        auto pcode = code_list.begin();
        auto pend = code_list.end();

        assert(pcode != pend);

        CodeLine* sline = *pcode++; assert(sline);
        Token* label = sline->label;
        String* name = GetText(label);

        assert(sline->instab->second.operation == Instruction::PSEUDO_DATA);
        if (!name)
            return parse_error(ErrorCode::NO_SECTION_LABEL, {sline->instr});
        if (dataidx_name->has(*name))
            return parse_error(ErrorCode::EXIST_DATA_NAME, {label});
        data->name = name;

        WASMGEN_DEBUG(2, "  label=\"", name->c_str(), "\"\n");

        ExpressionList* sops = sline->operands; assert(sops);
        bool active = false;
        bool set_memidx = false;
        int memidx = -1;

        auto opp = sops->begin();
        auto ope = sops->end();

        while (opp != ope)
        {
            Expression* op_memidx = *opp;
            auto mi_res = getvalue(op_memidx, memidx_name);

            if (!mi_res.isnumber())
                return parse_error(ErrorCode::NO_MEMORY_NAME, {*op_memidx});
            active = true;
            set_memidx = true;
            memidx = mi_res;

            WASMGEN_DEBUG(2, "  memidx=", memidx, "\n");

            if (++opp == ope)
                break;

            data->offset_expr = new CodeList;
            if (!make_expr(data->offset_expr, sline->instr, ValType::I32, int(opp - sops->begin()), opp, ope))
                return parse_error(ErrorCode::CAN_NOT_BE_INLINED, {**opp});
            add_symbol(SectionID::ELEMENT, data->offset_expr);

            WASMGEN_DEBUG(2, "  offset=", (*opp)->gettokencstr(), "\n");

            opp = ope;
            break;
        }
        if (opp != ope)
            parse_warning_too_many_operands(sline, **opp, opp - sops->begin());

        /**/

        Instruction::DataParamCode cdpc = Instruction::DPC_NONE;
        CodeListPtr binary_code;

        while (pcode != pend)
        {
            CodeLine* line = *pcode++; assert(line);
            auto& operation = line->instab->second.operation;

            if (operation != Instruction::Operation(Instruction::EPC_OFFSET) &&
                operation != Instruction::Operation(Instruction::DPC_MEMIDX))
            {
                switch (cdpc)
                {
                case Instruction::DPC_OFFSET:
                    if (operation == Instruction::INSTRUCTION ||
                        operation == Instruction::PSEUDO_EMPTY)
                    {
                        data->offset_expr->push_back(line);
                        break;
                    }
                    fallthrough;
                case Instruction::DPC_NONE:
                    if (operation == Instruction::PSEUDO_EMPTY ||
                        operation == Instruction::PSEUDO_BINARY)
                    {
                        if (!binary_code)
                        {
                            binary_code = new CodeList;
                            binary_code->binary = true;
                            add_symbol(SectionID::DATA, binary_code);
                        }
                        binary_code->push_back(line);
                        cdpc = Instruction::DPC_NONE;
                        break;
                    }
                    fallthrough;
                default:
                    return parse_error(ErrorCode::INVALID_DATA_CODE, {line->instr});
                }
                continue;
            }

            auto ndpc = Instruction::DataParamCode(operation);

            switch (ndpc)
            {
            case Instruction::DPC_MEMIDX:
                {
                    Expression* op0 = (*line->operands)[0]; assert(op0);
                    auto mi_res = getvalue(op0, memidx_name);

                    if (!mi_res.isnumber())
                        return parse_error(ErrorCode::NO_MEMORY_NAME, {*op0});
                    if (set_memidx && memidx != int(mi_res))
                        return parse_error(ErrorCode::TOO_MANY_MEMIDX, {*op0});
                    active = true;
                    set_memidx = true;
                    memidx = int(mi_res);

                    WASMGEN_DEBUG(2, "  memidx=", memidx, "\n");
                }
                break;

            case Instruction::DPC_OFFSET:
                {
                    if (!set_memidx)
                        return parse_error(ErrorCode::NOT_DATA_ACTIVE, {line->instr});
                    if (data->offset_expr)
                        return parse_error(ErrorCode::TOO_MANY_OFFSET, {line->instr});
                    data->offset_expr = new CodeList;
                    add_symbol(SectionID::DATA, data->offset_expr);

                    ExpressionList* ops = line->operands; assert(ops);

                    cdpc = ndpc;
                    if (ops->size())
                    {
                        if (!make_expr(data->offset_expr, sline->instr, ValType::I32, 0, ops->begin(), ops->end()))
                            return parse_error(ErrorCode::CAN_NOT_BE_INLINED, {*(*ops)[0]});
                        active = true;
                        cdpc = Instruction::DPC_NONE;
                    }
                }
                break;

            default:
                parse_bug_unknown_pseudo_code(line);
            }
        }

        data->active = active;
        if (set_memidx)
            data->memidx = memidx;
        if (binary_code)
            data->binary_code = Transfer(binary_code);

        data->index = int(append_index(module->dataidx, label));

        WASMGEN_DEBUG(2, "  index=", data->index, "\n");

        module->data_section->push_back(Transfer(data));
        return true;
    }

    /*
     *
     */

    bool Parser::update_code_list()
    {
        retry = false;
        return (true &&
                update_custom_section() &&
                update_function_section() &&
                update_global_section() &&
                update_element_section() &&
                update_data_section() &&
                true);
    }

    bool Parser::update_custom_section()
    {
        CustomSection* custom = module->custom_section; assert(custom);

        for (auto i : inc_range<size_t>(Section::size))
        {
            CustomSectionData* csdat = custom->section[i];

            if (csdat && !reparse_code_list(csdat->code))
                return false;
        }
        return true;
    }

    bool Parser::update_function_section()
    {
        FunctionSection* function = module->function_section; assert(function);

        for (FunctionCode* fcode : *function)
        {
            assert(fcode);
            if (!reparse_code_list(fcode->code))
                return false;
        }
        return true;
    }

    bool Parser::update_global_section()
    {
        GlobalSection* global = module->global_section; assert(global);

        for (GlobalData* data : *global)
        {
            assert(data);
            if (!reparse_code_list(data->code))
                return false;
        }
        return true;
    }

    bool Parser::update_element_section()
    {
        ElementSection* element = module->element_section; assert(element);

        for (ElementData* data : *element)
        {
            assert(data);
            if (!reparse_code_list(data->offset_expr))
                return false;
            for (ElementInit* init : *data->init)
            {
                assert(init);
                if (!reparse_code_list(init->code))
                    return false;
            }
        }
        return true;
    }

    bool Parser::update_data_section()
    {
        DataSection* dsec = module->data_section; assert(dsec);

        for (DataBlock* block : *dsec)
        {
            assert(block);
            if (!reparse_code_list(block->offset_expr) ||
                !reparse_code_list(block->binary_code))
                return false;
        }
        return true;
    }

    /*
     *
     */

    void Parser::add_symbol(SectionID id, CodeList* list)
    {
        IdentifierList* idmap = list->symbol;

        if (!idmap)
        {
            idmap = new IdentifierList;

            Identifier* label = list->label;

            if (!label)
            {
                label = new Identifier;
                list->label = label;
            }
            idmap->push_back(label);

            list->symbol = idmap;
        }
        switch (id)
        {
        case SectionID::TYPE:
            idmap->push_back(module->typeidx);
            break;
        case SectionID::FUNCTION:
            idmap->push_back(module->funcidx);
            break;
        case SectionID::TABLE:
            idmap->push_back(module->tableidx);
            break;
        case SectionID::MEMORY:
            idmap->push_back(module->memidx);
            break;
        case SectionID::GLOBAL:
            idmap->push_back(module->globalidx);
            break;
        case SectionID::ELEMENT:
            idmap->push_back(module->elemidx);
            break;
        case SectionID::DATA:
            idmap->push_back(module->dataidx);
            break;
        default:
            break;
        }
        idmap->push_back(alias_name);
    }

    /*
     *
     */

    bool Parser::reparse_code_list(CodeList* list)
    {
        if (!list)
            return true;
        if (!parse_code_list(list))
            return false;
        if (list->retry)
            retry = true;
        return true;
    }

    bool Parser::parse_code_list(CodeList* list)
    {
        assert(list);

        if (!list->br_idx)
        {
            IdentifierList* sym = list->symbol;
            Identifier* br_idx = new Identifier;

            if (!sym)
            {
                sym = new IdentifierList;

                sym->push_back(br_idx);
                sym->push_back(alias_name);

                list->symbol = sym;
            }

            list->br_idx = br_idx;
            sym->insert(sym->begin(), br_idx);
        }

        list->retry = false;
        list->offset = 0;
        list->code_end = nullptr;
        list->block_stack.clear();
        for (CodeLine* line : *list)
        {
            assert(line);
            if (!parse_code_line(list, line))
                break;
            list->offset += line->binary.size();
        }

        if (list->block_stack.size() && !list->pass && !error_count)
            for (CodeLine* line : list->block_stack)
                parse_warning(ErrorCode::NO_END, {line->instr});

        ++list->pass;
        return !error_count;
    }

    bool Parser::parse_code_line(CodeList* list, CodeLine* line)
    {
        assert(list);
        assert(line);

        auto& instab = line->instab;
        auto& ins = instab->second;
        auto& instr = ins.operation;

        if (list->code_end)
        {
            if (!list->pass)
                parse_warning(ErrorCode::IGNORE_CODE_AFTER_END, {line->instr});
            return true;
        }
        switch (Instruction::PseudoCode(instr))
        {
        case Instruction::INSTRUCTION:
            if (list->binary)
                return parse_error(ErrorCode::INVALID_INSTRUCTION, {line->instr});
            return parse_code_line_operand(list, line);

        case Instruction::PSEUDO_EMPTY:
        case Instruction::PSEUDO_BINARY:
            if (!list->binary)
                return parse_error(ErrorCode::INVALID_INSTRUCTION, {line->instr});
            return parse_data_line_operand(list, line);

        default:
            parse_bug_unknown_pseudo_code(line);
        }
    }

    bool Parser::parse_code_line_operand(CodeList* list, CodeLine* line)
    {
        WASMGEN_DEBUG(2, "  instr: ", GetCStr(line->instr), "\n");

        assert(list);
        assert(line);

        Token* label = line->label;
        auto& instab = line->instab;
        auto& binary = line->binary;
        auto& ins = instab->second;
        auto& instr = ins.operation;
        auto stm = instr.stack();
        bool use_label = false;

        switch (stm)
        {
        case Instruction::ST_NONE:
            break;

        case Instruction::ST_LEAVE_BLOCK:
        case Instruction::ST_LEAVE_LOOP:
        case Instruction::ST_LEAVE_IF:
        case Instruction::ST_LEAVE:
            check_operands(line, 0, 0);
            if (!list->block_stack.size())
            {
                if (!list->pass && stm != Instruction::ST_LEAVE)
                    parse_warning(ErrorCode::UNINTENTIONAL_END, {line->instr});
                list->code_end = line;
            }
            else do
            {
                CodeLineRef sline = list->block_stack.pop();

                if (list->pass)
                    break;
                if (!sline->label)
                {
                    sline->label = label;
                    use_label = true;
                }

                auto& sinstab = sline->instab;
                bool br_type = true;

                switch (stm)
                {
                case Instruction::ST_LEAVE_BLOCK:
                    br_type = sinstab == instr_block;
                    break;
                case Instruction::ST_LEAVE_LOOP:
                    br_type = sinstab == instr_loop;
                    break;
                case Instruction::ST_LEAVE_IF:
                    br_type = sinstab == instr_if || sinstab == instr_else;
                    break;
                default: // case Instruction::ST_LEAVE:
                    break;
                }
                if (br_type)
                    break;
                parse_warning(ErrorCode::UNINTENTIONAL_END, {line->instr});
            } while (false);

            if (!update_br_idx(list))
                return false;
            break;

        case Instruction::ST_ENTER:
            list->block_stack.push_back(line);
            if (!update_br_idx(list))
                return false;
            use_label = true;
            break;

        case Instruction::ST_ELSE:
            {
                auto& bstack = list->block_stack;

                if (!bstack.size() ||
                    bstack.back()->instab->first != *ins_if)
                    return parse_error(ErrorCode::NO_EXIST_IF, {line->instr});
            }
            break;

        default:
            throw BUG("未知のスタック操作(", instr.stack(), ")です。");
        }

        if (label && !use_label && !list->pass)
            parse_warning(ErrorCode::IGNORE_CODE_LABEL, {label});

        // auto& insname = instab->first;
        auto& insop = ins.operand;
        ExpressionList* ops = line->operands; assert(ops);
        ExprValueListList opsval(insop.count());
        auto opp = ops->begin();
        auto ope = ops->end();
        bool incomplete = false;

        for (size_t i = 0; i < insop.count(); ++i)
        {
            size_t oplen = ope - opp;

            auto& opval = opsval[i];
            auto opc = insop.operand(int(i));

            IdentifierList* idmap = nullptr;
            size_t cnt = 1;

            switch (opc)
            {
            case Instruction::OP_NONE:
                assert(false);
                return false;

            case Instruction::OP_I32:
            case Instruction::OP_I64:
            case Instruction::OP_F32:
            case Instruction::OP_F64:
                break;

            case Instruction::OP_VI08:
                cnt = clamp<size_t>(1, oplen, 16);
                break;
            case Instruction::OP_VI16:
                cnt = clamp<size_t>(1, oplen, 8);
                break;
            case Instruction::OP_VI32:
            case Instruction::OP_VF32:
                cnt = clamp<size_t>(1, oplen, 4);
                break;
            case Instruction::OP_VI64:
            case Instruction::OP_VF64:
                cnt = clamp<size_t>(1, oplen, 2);
                break;

            case Instruction::OP_VL:
                break;
            case Instruction::OP_VLT:
                cnt = 16;
                break;

            case Instruction::OP_DID:
                idmap = dataidx_name;
                break;
            case Instruction::OP_EID:
                idmap = elemidx_name;
                break;
            case Instruction::OP_FID:
                idmap = funcidx_name;
                break;
            case Instruction::OP_GID:
                idmap = globalidx_name;
                break;
            case Instruction::OP_LID:
                idmap = list->symbol;
                break;
            case Instruction::OP_MID:
                idmap = memidx_name;
                break;
            case Instruction::OP_SID:
                idmap = list->symbol;
                break;
            case Instruction::OP_SIDn:
                cnt = oplen;
                idmap = list->symbol;
                break;
            case Instruction::OP_TID:
            case Instruction::OP_TID1:
            case Instruction::OP_TID2:
                idmap = tableidx_name;
                break;
            case Instruction::OP_XID:
                idmap = typeidx_name;
                break;

            case Instruction::OP_REF:
                if (!oplen)
                    return parse_error_too_few_operands(line);
                {
                    GetTypeRes res;

                    if (!getreftype(res, *opp, idmap))
                        return false;
                    opval.push_back(ExprValue(res.valtype));

                    ++opp;
                    --oplen;
                    cnt = 0;
                }
                break;

            case Instruction::OP_VTn:
                cnt = oplen;
                idmap = typeidx_name;
                break;

            case Instruction::OP_BT:
                cnt = std::min<size_t>(1, oplen);
                idmap = typeidx_name;
                if (oplen > 1)
                {
                    auto rvt = gettype(valtype_dict, *opp, idmap);

                    if (rvt.name)
                    {
                        FuncType btype;

                        for (; opp != ope; ++opp)
                        {
                            GetTypeRes gvres;

                            if (!getvaltype(gvres, *opp))
                                return false;
                            btype.res.push_back(gvres.valtype);
                        }

                        size_t index = add_blocktype(btype);

                        opval.push_back(ExprValue(uint64_t(index)));

                        cnt = oplen = 0;
                        break;
                    }
                }
                if (oplen && **(*opp) == TokenID('{'))
                {
                    FuncType btype;

                    if (!getfunctype(btype, *opp))
                        return false;

                    ++opp;
                    --oplen;
                    cnt = 0;

                    if (!btype.arg.size() && !btype.res.size())
                        break;

                    size_t index = add_blocktype(btype);

                    opval.push_back(ExprValue(uint64_t(index)));
                }
                break;

            case Instruction::OP_MA0:
            case Instruction::OP_MA1:
            case Instruction::OP_MA2:
            case Instruction::OP_MA3:
            case Instruction::OP_MA4:
                cnt = std::min<size_t>(2, oplen);
                break;

            default:
                throw BUG("未知のオペランド コード(", int(opc), ")です。");
            }

            if (oplen < cnt)
                return parse_error_too_few_operands(line);

            for (auto n : inc_range<size_t>(cnt))
            {
                assert(opp != ope);
                UNUSED(n);

                auto res = getvalue(*opp, idmap);

                if (!res.isnumber())
                {
                    if (list->pass)
                        return parse_error(ErrorCode::NOT_VALUE, {**opp});
                    incomplete = true;
                }
                opval.push_back(res);
                ++opp;
            }
        }
        if (opp != ope)
            parse_warning_too_many_operands(line, ops, opp - ops->begin());
        if (incomplete)
        {
            WASMGEN_DEBUG(2, "  incomplete: ", GetCStr(line->instr), "\n");

            list->retry = true;
            return true;
        }

        bool mbc = false;

        binary.clear();
        for (auto n : inc_range<uint16_t>(ins.length))
        {
            auto code = ins.binary[n];

            WASMGEN_DEBUG(2, "  code: ", int(code), "\n");

            if (n == 0 && (code == 0xfc || code == 0xfd))
            {
                binary.push_back(uint8_t(code));
                mbc = true;
                continue;
            }
            if (mbc)
            {
                binary.append_leb128(uint8_t(code));
                mbc = false;
                continue;
            }
            if (code < 0x100)
            {
                binary.push_back(uint8_t(code));
                continue;
            }

            size_t opn = code - 0x100;
            auto& opval = opsval[opn];
            auto opc = insop.operand(int(opn));

            WASMGEN_DEBUG(2, "  opc: ", int(opc), "\n");

            switch (opc)
            {
            case Instruction::OP_VL:
                assert(opval.size() >= 1);
                binary.push_back(uint8_t(opval[0]));
                break;

            case Instruction::OP_I32:
                assert(opval.size() >= 1);
                binary.append_leb128(int32_t(opval[0]));
                break;

            case Instruction::OP_I64:
                assert(opval.size() >= 1);
                binary.append_leb128(int64_t(opval[0]));
                break;

            case Instruction::OP_F32:
                assert(opval.size() >= 1);
                binary.append_object(float32le_t(float(opval[0])));
                break;

            case Instruction::OP_F64:
                assert(opval.size() >= 1);
                binary.append_object(float64le_t(double(opval[0])));
                break;

            case Instruction::OP_VLT:
                assert(opval.size() >= 16);
                for (auto n : inc_range<size_t>(16))
                    binary.push_back(uint8_t(opval[n]));
                break;

            case Instruction::OP_VI08:
                assert(opval.size() >= 1);
                binary.append_operands<uint8le_t>(opval, 16);
                break;
            case Instruction::OP_VI16:
                assert(opval.size() >= 1);
                binary.append_operands<uint16le_t>(opval, 8);
                break;
            case Instruction::OP_VI32:
                assert(opval.size() >= 1);
                binary.append_operands<uint32le_t>(opval, 4);
                break;
            case Instruction::OP_VI64:
                assert(opval.size() >= 1);
                binary.append_operands<uint64le_t>(opval, 2);
                break;
            case Instruction::OP_VF32:
                assert(opval.size() >= 1);
                binary.append_operands<float32le_t>(opval, 4);
                break;
            case Instruction::OP_VF64:
                assert(opval.size() >= 1);
                binary.append_operands<float64le_t>(opval, 2);
                break;

            case Instruction::OP_DID:
            case Instruction::OP_EID:
            case Instruction::OP_FID:
            case Instruction::OP_GID:
            case Instruction::OP_LID:
            case Instruction::OP_MID:
            case Instruction::OP_SID:
            case Instruction::OP_TID:
            case Instruction::OP_TID1:
            case Instruction::OP_TID2:
            case Instruction::OP_XID:
                assert(opval.size() >= 1);
                binary.append_leb128(uint32_t(opval[0]));
                break;
            case Instruction::OP_SIDn:
                binary.append_leb128(opval.size());
                for (auto n : inc_range<size_t>(opval.size()))
                    binary.append_leb128(uint32_t(opval[n]));
                break;

            case Instruction::OP_REF:
                assert(opval.size() >= 1);
                binary.append_leb128(int8_t(opval[0]));
                break;

            case Instruction::OP_VTn:
                if (opval.size())
                {
                    binary.append_leb128(opval.size());
                    for (auto n : inc_range<size_t>(opval.size()))
                        binary.append_leb128(int8_t(opval[n]));
                }
                else
                {
                    assert(binary.back() == 0x1c);
                    binary.back() = 0x1b;
                }
                break;

            case Instruction::OP_BT:
                binary.append_leb128(!opval.size()
                                           ? int64_t(ValType::EMPTY)
                                           : int64_t(opval[0]));
                break;

            case Instruction::OP_MA0:
            case Instruction::OP_MA1:
            case Instruction::OP_MA2:
            case Instruction::OP_MA3:
            case Instruction::OP_MA4:
                {
                    uint32_t offset = 0;
                    uint32_t align = uint32_t(opc - Instruction::OP_MA0);
                    switch (opval.size())
                    {
                    case 0:
                        break;
                    case 2:
                        align = opval[1];
                        fallthrough;
                    case 1:
                        offset = opval[0];
                        break;
                    default:
                        assert(false);
                        break;
                    }
                    binary.append_leb128(uint32_t(align));
                    binary.append_leb128(uint32_t(offset));
                }
                break;

            default:
                throw BUG("未知のオペランド コード(", int(opc), ")です。");
            }
        }

        return true;
    }

    bool Parser::parse_data_line_operand(CodeList* list, CodeLine* line)
    {
        WASMGEN_DEBUG(2, "  instr: ", GetCStr(line->instr), "\n");

        assert(list);
        assert(line);

        auto& instab = line->instab;
        auto& binary = line->binary;
        auto& ins = instab->second;
        auto& instr = ins.operation;

        Instruction::BinaryCode bcode = Instruction::BinaryCode(instr);
        Token* label = line->label;

        if (label)
        {
            Identifier* idmap = list->label; assert(idmap);
            String* name = label->text; assert(name);
            auto npos = idmap->find(*name);
            bool has = npos != idmap->end();

            if (has && !list->pass)
                return parse_error(ErrorCode::EXIST_CODE_LABEL, {label});

            ExpressionRef nexpr(make_value(label, list->offset));

            if (has)
            {
                Expression* cexpr = npos->second;

                if (cexpr->token->ivalue != nexpr->token->ivalue)
                    list->retry = true;
                npos->second = Transfer(nexpr);
            }
            else
                (*idmap)[*name] = Transfer(nexpr);

            WASMGEN_DEBUG(2, "  label[\"", *name, "\"] = ", list->offset, "\n");
        }

        switch (bcode)
        {
        case Instruction::BD_NONE:
            break;

        case Instruction::BD_ALIGN:
        case Instruction::BD_BALIGN:
            {
                if (!check_operands(line, 1, 2))
                    return false;

                ExpressionList* ops = line->operands;
                Expression* op0 = (*ops)[0]; assert(op0);
                auto ares = getvalue(op0, list->symbol);
                int64_t padding = 0;

                if (!ares.isnumber())
                    return parse_error(ErrorCode::NOT_VALUE, {*op0});

                if (ops->size() > 1)
                {
                    Expression* op1 = (*ops)[1]; assert(op1);
                    auto pres = getvalue(op1, list->symbol);

                    if (!pres.isnumber())
                        return parse_error(ErrorCode::NOT_VALUE, {*op1});

                    padding = pres;
                    if (!in_range<int64_t>(-128, padding, 255))
                        parse_warning(ErrorCode::TRUNCATED_VALUE, {*op1});
                }

                int64_t aval = ares;

                if (instr == Instruction::BD_ALIGN)
                {
                    if (!in_range<int64_t>(0, aval, 16))
                        return parse_error(ErrorCode::OUT_OF_RANGE, {*op0});

                    aval = int64_t(1) << aval;
                }
                else if (!in_range<int64_t>(1, aval, 65536))
                    return parse_error(ErrorCode::OUT_OF_RANGE, {*op0});

                size_t pos = list->offset;
                size_t rem = (aval - (pos % aval)) % aval;

                binary.clear();
                binary.resize(rem, uint8_t(padding));
            }
            break;

        case Instruction::BD_I8:
        case Instruction::BD_I8_BE:
        case Instruction::BD_I16:
        case Instruction::BD_I16_BE:
        case Instruction::BD_I32:
        case Instruction::BD_I32_BE:
        case Instruction::BD_I64:
        case Instruction::BD_I64_BE:
        case Instruction::BD_F32:
        case Instruction::BD_F32_BE:
        case Instruction::BD_F64:
        case Instruction::BD_F64_BE:
        case Instruction::BD_SLEB:
        case Instruction::BD_ULEB:
            return parse_data_line_binary(list, line);

        case Instruction::BD_STRING:
        case Instruction::BD_NSTRING:
            {
                if (!check_operands(line, 1, 1))
                    return false;

                ExpressionList* ops = line->operands;
                Expression* op0 = (*ops)[0]; assert(op0);
                auto ares = getvalue(op0, list->symbol);

                if (!ares.isstring())
                    return parse_error(ErrorCode::NOT_STRING, {*op0});

                const String* text = ares; assert(text);

                binary.clear();
                if (bcode == Instruction::BD_NSTRING)
                    binary.append_leb128(uint32_t(text->size()));
                binary.append_bytes(text->c_str(), text->size());
            }
            break;

        default:
            parse_bug_unknown_pseudo_code(line);
        }
        return true;
    }

    bool Parser::parse_data_line_binary(CodeList* list, CodeLine* line)
    {
        assert(list);
        assert(line);

        auto& instab = line->instab;
        auto& binary = line->binary;
        auto& ins = instab->second;
        auto& instr = ins.operation;

        Instruction::BinaryCode bcode = Instruction::BinaryCode(instr);
        size_t byte_size = size_t(1) << ((bcode - Instruction::BD_I8) >> 1);
        bool big_endian = (bcode - Instruction::BD_I8) & 1;
        bool floating = false;

        switch (bcode)
        {
        case Instruction::BD_I8:
        case Instruction::BD_I8_BE:
        case Instruction::BD_I16:
        case Instruction::BD_I16_BE:
        case Instruction::BD_I32:
        case Instruction::BD_I32_BE:
        case Instruction::BD_I64:
        case Instruction::BD_I64_BE:
            break;
        case Instruction::BD_F32:
        case Instruction::BD_F32_BE:
            byte_size = 4;
            floating = true;
            break;
        case Instruction::BD_F64:
        case Instruction::BD_F64_BE:
            byte_size = 8;
            floating = true;
            break;
        case Instruction::BD_SLEB:
        case Instruction::BD_ULEB:
            big_endian = false;
            byte_size = 0;
            break;

        default:
            throw BUG("未知のバイナリ コード(", int(bcode), ")です。");
        }

        ExpressionList* ops = line->operands; assert(ops);

        binary.clear();
        for (Expression* op : *ops)
        {
            auto res = getvalue(op, list->symbol);

            if (!res.isnumber())
            {
                if (list->pass)
                    return parse_error(ErrorCode::NOT_VALUE, {*op});
                if (byte_size)
                    binary.insert(binary.end(), byte_size, 0);
                list->retry = true;
                continue;
            }

            switch (byte_size)
            {
            case 0:
                if (bcode == Instruction::BD_SLEB)
                    binary.append_leb128(int64_t(res));
                else
                    binary.append_leb128(uint64_t(res));
                break;

            case 1:
                binary.push_back(uint8_t(res));
                break;

            case 2:
                if (big_endian)
                    binary.append_object(int16be_t(res));
                else
                    binary.append_object(int16le_t(res));
                break;

            case 4:
                if (!floating)
                {
                    if (big_endian)
                        binary.append_object(int32be_t(res));
                    else
                        binary.append_object(int32le_t(res));
                }
                else
                {
                    if (big_endian)
                        binary.append_object(float32be_t(res));
                    else
                        binary.append_object(float32le_t(res));
                }
                break;

            case 8:
                if (!floating)
                {
                    if (big_endian)
                        binary.append_object(int64be_t(res));
                    else
                        binary.append_object(int64le_t(res));
                }
                else
                {
                    if (big_endian)
                        binary.append_object(float64be_t(res));
                    else
                        binary.append_object(float64le_t(res));
                }
                break;

            default:
                throw BUG("未知のバイナリ サイズ(", byte_size, ")です。");
            }
        }
        return true;
    }

    /*
     *
     */

    size_t Parser::add_functype(FuncType& functype)
    {
        TypeSection* mod_type = module->type_section; assert(mod_type);
        auto& mod_index = mod_type->index;
        size_t index = mod_type->type.size();
        auto it = mod_index.find(functype);

        if (it != mod_index.end())
        {
            if (option.type_unique)
                return it->second;
        }
        else
        {
            if (option.type_unique)
                index = mod_index.size();
            mod_index[functype] = int(index);
        }
        assert(index == mod_type->type.size());
        mod_type->type.push_back(functype);
        return index;
    }

    size_t Parser::add_blocktype(FuncType& btype)
    {
        TypeSection* mod_type = module->type_section; assert(mod_type);
        auto& mod_index = mod_type->index;
        auto& mod_bindex = mod_type->bindex;
        FuncTypeMap::iterator it;

        it = mod_index.find(btype);
        if (it != mod_index.end())
            return it->second;

        it = mod_bindex.find(btype);
        if (it != mod_bindex.end())
            return it->second;

        size_t index = mod_type->type.size() + mod_type->btype.size();

        mod_bindex[btype] = int(index);
        mod_type->btype.push_back(btype);
        return index;
    }

    void Parser::append_index(Identifier* idmap, Token* token, size_t index)
    {
        assert(idmap);
        assert(token);

        String* name = token->text; assert(name);

        if (idmap->has(*name))
            message("exist: ", *name, "\n");

        assert(!idmap->has(*name));

        ExpressionRef expr(make_value(token, index));

        (*idmap)[*name] = Transfer(expr);
    }

    bool Parser::update_br_idx(CodeList* list)
    {
        Identifier* idmap = list->br_idx; assert(idmap);
        auto& bstack = list->block_stack;
        size_t index = bstack.size();

        idmap->clear();
        for (CodeLine* line : bstack)
        {
            assert(line);
            --index;

            Token* label = line->label;

            if (label)
            {
                String* name = label->text; assert(name);

                if (idmap->has(*name))
                    return parse_error_too_few_operands(label);
                append_index(idmap, label, index);
            }
        }
        return true;
    }

    /*
     *
     */

    ExprValue Parser::get_asmsw_value(const char* name)
    {
        ExprValue res;
        Expression* expr = define_name->getexpr(name);

        if (expr)
            res = getvalue(expr);
        return res;
    }

    ExprValue Parser::getvalue(StdStringSet& nest, Expression* expr, IdentifierList* label)
    {
        assert(expr && expr->children);

        auto& children = *expr->children;
        Token* et = expr->token; assert(et);

        switch (expr->mode)
        {
        case Expression::VALUE:
            {
                if (et->id == TokenID::NUMBER)
                    return et->isfloat ? ExprValue(et->fvalue) : ExprValue(et->ivalue);
                if (et->id == TokenID::QUOTE)
                    return ExprValue(&et->quote);
                if (et->id == TokenID::NAME && label)
                {
                    String* text = et->text; assert(text);
                    auto p = label->getexpr(*text);

                    if (p)
                    {
                        if (nest.has(*text))
                        {
                            parse_error(ErrorCode::INFINITE_LOOP, {et});
                            return ExprValue();
                        }
                        nest.insert(*text);

                        auto r = getvalue(nest, p, label);

                        if (r.isvalid())
                            return r;
                    }

                    if (*text == "null")
                        return ExprValue(ExprValue::ST_NULL);
                    {
                        auto vp = valtype_dict.find(*text);

                        if (vp != valtype_dict.end())
                            return ExprValue(vp->second);
                    }
                    return ExprValue(text, ExprValue::ST_NAME);
                }
            }
            break;

        case Expression::UNARY:
            {
                assert(children.size() >= 1);

                auto v0 = getvalue(nest, children[0], label);

                if (!v0.isnumber())
                    break;

                switch (et->id)
                {
                case TokenID::BNOT:
                    if (v0.isnumber())
                        return ExprValue(!bool(v0));
                    break;

                case TokenID::NOT:
                    if (v0.isnumber())
                        return ExprValue(~int64_t(v0));
                    break;

                case TokenID::ADD:
                    return v0;

                case TokenID::SUB:
                    if (v0.isint())
                        return ExprValue(-int64_t(v0));
                    if (v0.isfloat())
                        return ExprValue(-double(v0));
                    break;

                case TokenID::MUL:
                    parse_error(ErrorCode::SYNTAX_ERROR, {et});
                    break;

                default:
                    throw BUG("未知の単項演算子 \"", (const char*)(*et->text), "\" です。");
                }
            }
            break;

        case Expression::BINARY:
            {
                assert(children.size() >= 2);

                Expression* expr0 = children[0];
                auto v0 = getvalue(nest, expr0, label);

                if (!v0.isvalid())
                    break;

                Expression* expr1 = children[1];
                auto v1 = getvalue(nest, expr1, label);

                if (!v1.isvalid())
                    break;

                switch (et->id)
                {
                case TokenID::CMPLT:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpLT());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string < *v1.string);
                    break;

                case TokenID::CMPLE:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpLE());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string <= *v1.string);
                    break;

                case TokenID::CMPEQ:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpEQ());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string == *v1.string);
                    break;

                case TokenID::CMPNE:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpNE());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string != *v1.string);
                    break;

                case TokenID::CMPGE:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpGE());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string >= *v1.string);
                    break;

                case TokenID::CMPGT:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpGT());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(*v0.string > *v1.string);
                    break;

                case TokenID::ADD:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpAdd());
                    if (v0.isstring() && v1.isstring())
                        return ExprValue(new String(*v0.string + *v1.string));
                    break;

                case TokenID::SUB:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpSub());
                    break;

                case TokenID::MUL:
                    if (v0.isnumber() && v1.isnumber())
                        return binary_operator(v0, v1, OpMul());
                    if (v0.islist() && v1.isnumber())
                    {
                        auto& vl = v0.list;

                        if (vl.size() != 1)
                            break;

                        auto& ve = vl[0];
                        ExprValue r(ExprValue::ST_LIST);
                        auto& rl = r.list;

                        for (auto n : inc_range<size_t>(uint64_t(v1)))
                        {
                            UNUSED(n);

                            rl.push_back(ve);
                        }
                        return r;
                    }
                    {
                        int64_t n = -1;
                        String* q = nullptr;

                        if (v0.isnumber() && v1.isstring())
                        {
                            n = v0;
                            q = v1.string;
                        }
                        else if (v0.isstring() && v1.isnumber())
                        {
                            n = v1;
                            q = v0.string;
                        }
                        else
                            break;

                        String *s = new String;

                        for (int64_t i = 0; i < n; i++)
                            *s += *q;
                        return ExprValue(s);
                    }
                    break;

                case TokenID::DIV:
                    if (v0.isnumber() && v1.isnumber())
                    {
                        if (bool(v1))
                            return binary_operator(v0, v1, OpDiv());
                        parse_error(ErrorCode::DIVISION_BY_ZERO, {*expr1});
                    }
                    break;

                case TokenID::MOD:
                    if (v0.isnumber() && v1.isnumber())
                    {
                        if (bool(v1))
                            return v1.isfloat()
                                ? ExprValue(std::fmod(double(v0), v1.fvalue))
                                : ExprValue(int64_t(v0) % v1.ivalue);
                        parse_error(ErrorCode::DIVISION_BY_ZERO, {*expr1});
                    }
                    break;

                    /**/

                case TokenID::AND:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(v0) & int64_t(v1));
                    break;

                case TokenID::XOR:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(v0) ^ int64_t(v1));
                    break;

                case TokenID::OR:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(v0) | int64_t(v1));
                    break;

                case TokenID::LSHIFT:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(v0) << int64_t(v1));
                    break;

                    /**/

                case TokenID::RSHIFT:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(v0) >> int64_t(v1));
                    break;

                case TokenID::RSHIFTU:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(int64_t(uint64_t(int64_t(v0)) >> int64_t(v1)));
                    break;

                    /**/

                case TokenID::BAND:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(bool(v0) && bool(v1));
                    break;

                case TokenID::BOR:
                    if (v0.isnumber() && v1.isnumber())
                        return ExprValue(bool(v0) || bool(v1));
                    break;

                    /**/

                case TokenID::VADD:
                    if (v0.islist() && v1.islist())
                        return binary_operator(v0, v1, OpAdd(), et);
                    break;

                case TokenID::VSUB:
                    if (v0.islist() && v1.islist())
                        return binary_operator(v0, v1, OpSub(), et);
                    break;

                case TokenID::VMUL:
                    if (v0.islist() && v1.islist())
                        return binary_operator(v0, v1, OpMul(), et);
                    break;

                case TokenID::VDIV:
                    if (v0.islist() && v1.islist())
                        return binary_operator(v0, v1, OpDiv(), et);
                    break;

                    /**/

                case TokenID::EQUAL:
                    parse_error(ErrorCode::SYNTAX_ERROR, {et});
                    break;

                default:
                    throw BUG("未知の二項演算子 \"", (const char*)(*et->text), "\" です。");
                }
            }
            break;

        case Expression::CONDITIONAL:
            {
                assert(children.size() >= 3);

                auto v0 = getvalue(nest, children[0], label);

                if (v0.isnumber())
                    return getvalue(nest, children[bool(v0) ? 1 : 2], label);
            }
            break;

        case Expression::LIST:
            {
                ExprValue r(ExprValue::ST_LIST);
                ExprValueList& rl = r.list;

                for (Expression* op : children)
                {
                    assert(op);

                    auto v = getvalue(nest, op, label);

                    if (!v.isvalid())
                        return ExprValue();
                    rl.push_back(v);
                }
                return r;
            }
            break;

        case Expression::ITEM:
            {
                assert(children.size() >= 2);

                Expression* op0 = children[0]; assert(op0);

                auto v1 = getvalue(nest, children[1], label);

                if (!v1.islist())
                    break;
                if (v1.list.size() != 1)
                    break;

                auto& vp = v1.list[0];

                if (!vp.isnumber())
                    break;

                int64_t pos(vp);
                auto v0 = getvalue(nest, children[0], label);

                if (v0.islist())
                {
                    auto& vl = v0.list;

                    if (pos < 0)
                        pos += int64_t(vl.size());
                    if (pos < 0 || int64_t(vl.size()) <= pos)
                    {
                        parse_error(ErrorCode::LIST_INDEX_OVERFLOW, {et});
                        break;
                    }
                    return vl[size_t(pos)];
                }
            }
            break;

        default:
            break;
        }
        return ExprValue();
    }

    String* Parser::getstring(StdStringSet& nest, Expression* expr, IdentifierList* label, bool plain, bool quote)
    {
        if (expr->mode != Expression::VALUE)
            return nullptr;

        Token* et = expr->token; assert(et);

        if (quote && et->id == TokenID::QUOTE)
            return et->quote;

        String* text = et->text; assert(text);

        assert(label);
        if (et->id != TokenID::NAME)
            return nullptr;

        auto p = label->getexpr(*text);

        if (!p)
            return plain ? text : nullptr;

        if (nest.has(*text))
        {
            parse_error(ErrorCode::INFINITE_LOOP, {et});
            return nullptr;
        }
        nest.insert(*text);

        return getstring(nest, p, label, plain, quote);
    }

    /*
     *
     */

    inline bool Parser::getvaltype(GetTypeRes& res, Expression* expr, IdentifierList* table)
    {
        assert(expr);
        res = gettype(valtype_dict, expr, table);
        if (!res.name)
            return parse_error(ErrorCode::NOT_VALTYPE, {*expr});
        return true;
    }

    bool Parser::getvaltypes(ResultType& res, ExpressionList* ops)
    {
        res.clear();
        for (Expression* op : *ops)
        {
            assert(op);

            GetTypeRes gvres;

            if (!getvaltype(gvres, op))
                return false;
            res.push_back(gvres.valtype);

            WASMGEN_DEBUG(2, "  valtype: ", op->getnamecstr(), "=", int(gvres.valtype), "\n");
        }
        return true;
    }

    bool Parser::getlimits(Limits& limits, CodeLine* line, size_t pos, IdentifierList* table)
    {
        assert(line);

        ExpressionList* ops = line->operands; assert(ops);

        limits = Limits(0);
        if (pos >= ops->size())
            return parse_error(ErrorCode::NO_LIMITS, {line->instr});
        {
            Expression* op = (*ops)[pos]; assert(op);
            auto vmin = getvalue(op, table);

            if (!vmin.isnumber())
                return parse_error(ErrorCode::NOT_VALUE, {*op});
            limits.min = vmin;
        }
        if ((pos + 1) < ops->size())
        {
            Expression* op = (*ops)[pos + 1]; assert(op);
            auto vmax = getvalue(op, table);

            if (!vmax.isnumber())
                return parse_error(ErrorCode::NOT_VALUE, {*op});
            limits.mode = 1;
            limits.max = vmax;
        }
        return true;
    }

    bool Parser::getfunctype(FuncType& res, Expression* expr)
    {
        assert(expr);

        Token* token = expr->token; assert(token);

        if (expr->mode != Expression::LIST || token->id != TokenID('{'))
            return parse_error(ErrorCode::SYNTAX_ERROR, {token});

        ExpressionList* children = expr->children; assert(children);

        if (children->size() < 2)
            return parse_error(ErrorCode::SYNTAX_ERROR, {*expr});
        if (children->size() > 2)
            parse_warning(ErrorCode::IGNORE_FUNCTYPE_PARAMETER, {*(*children)[2]});

        if (!getfunctype_resulttypes(res.arg, (*children)[0]))
            return false;
        if (!getfunctype_resulttypes(res.res, (*children)[1]))
            return false;
        return true;
    }

    bool Parser::getfunctype_resulttypes(ResultType& res, Expression* expr)
    {
        assert(expr);

        Token* token = expr->token; assert(token);

        res.clear();
        if (expr->mode == Expression::LIST)
        {
            if (token->id != TokenID('['))
                return parse_error(ErrorCode::SYNTAX_ERROR, {token});
            if (!getvaltypes(res, expr->children))
                return false;
        }
        else
        {
            GetTypeRes gvres;

            if (!getvaltype(gvres, expr))
                return false;
            res.push_back(gvres.valtype);
        }
        return true;
    }

    /*
     *
     */

    bool Parser::getequal(GetEqualRes& res, Expression* expr, IdentifierList* table)
    {
        Token* token = expr->token; assert(token);

        res.name = nullptr;
        if (token->id != TokenID('='))
        {
            parse_error(ErrorCode::SYNTAX_ERROR, {token});
            return false;
        }
        assert(expr->children->size() == 2);

        Expression* op_name = (*expr->children)[0]; assert(op_name);
        Expression* op_value = (*expr->children)[1]; assert(op_name);
        Token* tname = op_name->token; assert(tname);

        if (tname->id != TokenID::NAME)
        {
            parse_error(ErrorCode::SYNTAX_ERROR, {tname});
            return false;
        }
        res.name = tname;
        res.value = getvalue(op_value, table);
        return res.value.isvalid();
    }

    bool Parser::geteqval(GetEqualRes& res, Expression* expr, IdentifierList* table)
    {
        if (!getequal(res, expr, table))
            return false;
        if (!res.value.isnumber())
            return parse_error(ErrorCode::NOT_VALUE, {*(*expr->children)[1]});
        return true;
    }

    bool Parser::geteqstr(GetEqualRes& res, Expression* expr, IdentifierList* table)
    {
        if (!getequal(res, expr, table))
            return false;
        if (!res.value.isstring())
            return parse_error(ErrorCode::NOT_STRING, {*(*expr->children)[1]});
        return true;
    }

    bool Parser::getreftype(GetTypeRes& res, Expression* expr, IdentifierList* table)
    {
        res = gettype(reftype_dict, expr, table);
        if (!res.name)
            return parse_error(ErrorCode::NO_TYPE_NAME, {*expr});
        if (res.valtype == RefType::NONE)
            return parse_error(ErrorCode::NOT_REFTYPE, {*expr});
        return true;
    }

    TypeData* Parser::gettypedata(Expression* expr)
    {
        String* name = getname(expr, global_name);

        if (name)
        {
            auto& table = module->type_section->name;
            auto it = table.find(*name);
            if (it != table.end())
                return it->second;
        }
        return nullptr;
    }

    /*
     *
     */

    bool Parser::make_expr(CodeList* code, Token* token, ValType valtype,
                           int opstart, ExprConstIter opb, ExprConstIter ope)
    {
        InstrIter instab = instr_dummy;

        switch (valtype)
        {
        case ValType::I32:
            instab = instr_i32_const;
            break;
        case ValType::I64:
            instab = instr_i64_const;
            break;
        case ValType::F32:
            instab = instr_f32_const;
            break;
        case ValType::F64:
            instab = instr_f64_const;
            break;
        case ValType::V128:
            instab = instr_v128_const;
            break;
        default:
            return false;
        }
        return make_expr(code, token, instab->first, instab, opstart, opb, ope);
    }

    bool Parser::make_expr(CodeList* code, Token* token,
                           const StdString& instr, InstrIter instab,
                           int opstart, ExprConstIter opb, ExprConstIter ope)
    {
        NewCodeLine code_const(new ExpressionList(opb, ope), opstart);
        NewCodeLine code_end;

        code_const->instr = make_token(token, instr);
        code_const->instab = instab;
        code->push_back(Transfer(code_const));

        code_end->instr = make_token(token, *ins_end);
        code_end->instab = instr_end;
        code->push_back(Transfer(code_end));
        return true;
    }

    /*
     *
     */

    StdString Parser::make_error_message(ErrorCode code, const char* prefix, const PTokenList& token_list)
    {
        auto& tab = error_code_table[code];
        auto& errmsg = tab.messages;
        StdString message(StringBlock{prefix, unsigned(code), ": "});

        for (auto pos : tab.order)
        {
            if (pos < 0)
            {
                pos = -pos - 1;
                assert(size_t(pos) < token_list.size());

                Token* token = token_list[pos];

                if (Valid(token))
                    message += *token->text;
                else
                    message += "[EMPTY]";
            }
            else
            {
                assert(size_t(pos) < errmsg.size());
                message += errmsg[pos];
            }
        }
        return message;
    }

    bool Parser::parse_error(ErrorCode code, const PTokenList& token_list)
    {
        const Token* token = token_list.size() ? token_list[0] : nullptr;
        StdString message = make_error_message(code, "error:E", token_list);

        assert(token);
        parse_message(token, message.c_str());
        ++error_count;
        return false;
    }

    bool Parser::parse_expr_error(ErrorCode code, const PTokenList& token_list)
    {
        if (!expr_error)
            return parse_error(code, token_list);
        expr_error = true;
        return false;
    }

    void Parser::parse_warning(ErrorCode code, const PTokenList& token_list)
    {
        const Token* token = token_list.size() ? token_list[0] : nullptr;
        StdString message = make_error_message(code, "warning:W", token_list);

        parse_message(token, message.c_str());
    }

    void Parser::parse_warning_too_many_operands(CodeLine* line, Token* token, size_t n)
    {
        TokenPtr number = make_token(token, n + 1 + line->operand_start);

        parse_warning(ErrorCode::TOO_MANY_OPERANDS, {token, &number});
    }

    /*
     *
     */

    void Parser::build_wasm(ModuleBinary& wasm)
    {
        wasm.clear();
        wasm.append_bytes("\0asm", 4);      // magic
        wasm.append_object(uint32le_t(1));  // version
        put_custom_section(wasm);
        put_type_section(wasm);
        put_import_section(wasm);
        put_function_section(wasm);
        put_table_section(wasm);
        put_memory_section(wasm);
        put_global_section(wasm);
        put_export_section(wasm);
        put_start_section(wasm);
        put_element_section(wasm);
        put_datacount_section(wasm);
        put_code_section(wasm);
        put_data_section(wasm);
    }

    void Parser::put_custom_section(ModuleBinary& wasm, SectionID id)
    {
        CustomSection* sec = module->custom_section; assert(sec);
        CustomSectionData* cdata = sec->section[int(id)];

        if (!cdata)
            return;
        assert(cdata->id == id);

        SectionBinary wasmsec;

        wasmsec.append_name(*cdata->name);
        if (cdata->code)
            wasmsec.append_code(cdata->code);
        wasm.append_section(SectionID::CUSTOM, wasmsec);
    }

    void Parser::put_type_section(ModuleBinary& wasm)
    {
        TypeSection* sec = module->type_section; assert(sec);
        size_t type_size = sec->type.size() + sec->btype.size();

        if (type_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(type_size));
            for (auto& ftype : sec->type)
                wasmsec.append_functype(ftype);
            for (auto& btype : sec->btype)
                wasmsec.append_functype(btype);
            wasm.append_section(SectionID::TYPE, wasmsec);
        }
        put_custom_section(wasm, SectionID::TYPE);
    }

    void Parser::put_import_section(ModuleBinary& wasm)
    {
        ImportSection* sec = module->import_section; assert(sec);
        size_t import_size = sec->size();

        if (import_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(import_size));
            for (ImportData* imp : *sec)
            {
                wasmsec.append_name(*imp->mod);
                wasmsec.append_name(*imp->name);
                wasmsec.push_back(imp->desc);
                switch (imp->desc)
                {
                case ImportData::FUNCTION:
                    wasmsec.append_label_index(imp->typeidx);
                    break;
                case ImportData::TABLE:
                    wasmsec.append_valtype(imp->valtype);
                    fallthrough;
                case ImportData::MEMORY:
                    wasmsec.append_limits(imp->limits);
                    break;
                case ImportData::GLOBAL:
                    wasmsec.append_valtype(imp->valtype);
                    wasmsec.push_back(uint8_t(imp->mut));
                    break;
                default:
                    throw BUG("未知の import 記述子(", imp->desc, ")です。");
                }
            }
            wasm.append_section(SectionID::IMPORT, wasmsec);
        }
        put_custom_section(wasm, SectionID::IMPORT);
    }

    void Parser::put_function_section(ModuleBinary& wasm)
    {
        FunctionSection* sec = module->function_section; assert(sec);
        size_t function_size = sec->size();

        if (function_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(function_size));
            for (FunctionCode* fcode : *sec)
            {
                TypeData* data = fcode->type; assert(data);

                wasmsec.append_label_index(data->index);
            }
            wasm.append_section(SectionID::FUNCTION, wasmsec);
        }
        put_custom_section(wasm, SectionID::FUNCTION);
    }

    void Parser::put_table_section(ModuleBinary& wasm)
    {
        TableSection* sec = module->table_section; assert(sec);
        size_t table_size = sec->size();

        if (table_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(table_size));
            for (TableData* tdata : *sec)
            {
                wasmsec.append_valtype(tdata->reftype);
                wasmsec.append_limits(tdata->limits);
            }
            wasm.append_section(SectionID::TABLE, wasmsec);
        }
        put_custom_section(wasm, SectionID::TABLE);
    }

    void Parser::put_memory_section(ModuleBinary& wasm)
    {
        MemorySection* sec = module->memory_section; assert(sec);
        size_t memory_size = sec->size();

        if (memory_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(memory_size));
            for (MemoryData* mdata : *sec)
                wasmsec.append_limits(mdata->limits);
            wasm.append_section(SectionID::MEMORY, wasmsec);
        }
        put_custom_section(wasm, SectionID::MEMORY);
    }

    void Parser::put_global_section(ModuleBinary& wasm)
    {
        GlobalSection* sec = module->global_section; assert(sec);
        size_t global_size = sec->size();

        if (global_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(global_size));
            for (GlobalData* gdata : *sec)
            {
                wasmsec.append_valtype(gdata->valtype);
                wasmsec.push_back(uint8_t(gdata->mut));
                wasmsec.append_code(gdata->code);
            }
            wasm.append_section(SectionID::GLOBAL, wasmsec);
        }
        put_custom_section(wasm, SectionID::GLOBAL);
    }

    void Parser::put_export_section(ModuleBinary& wasm)
    {
        ExportSection* sec = module->export_section; assert(sec);
        size_t export_size = sec->size();

        if (export_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(export_size));
            for (ExportData* edata : *sec)
            {
                int desc(edata->desc);

                assert(0 <= desc && desc <= 3);
                wasmsec.append_name(edata->name);
                wasmsec.push_back(uint8_t(desc));
                wasmsec.append_leb128(uint32_t(edata->index));
            }
            wasm.append_section(SectionID::EXPORT, wasmsec);
        }
        put_custom_section(wasm, SectionID::EXPORT);
    }

    void Parser::put_start_section(ModuleBinary& wasm)
    {
        StartSection* sec = module->start_section; assert(sec);

        if (sec->funcidx >= 0)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(sec->funcidx));
            wasm.append_section(SectionID::START, wasmsec);
        }
        put_custom_section(wasm, SectionID::START);
    }

    void Parser::put_element_section(ModuleBinary& wasm)
    {
        ElementSection* sec = module->element_section; assert(sec);
        size_t element_size = sec->size();

        if (element_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(element_size));
            for (ElementData* edata : *sec)
            {
                uint32_t emode = 0;

                if (edata->funcidx_expr ||
                    edata->reftype != RefType::FUNCREF)
                    emode |= 4;

                switch (edata->mode)
                {
                case ElementMode::ACTIVE:
                    if (edata->tableidx >= 0)
                        emode |= 2;
                    break;
                case ElementMode::DECLARATIVE:
                    emode |= 3;
                    break;
                case ElementMode::PASSIVE:
                    emode |= 1;
                    break;
                default:
                    throw BUG("未知の Element 種(", int(edata->mode), ")です。");
                }

                uint8_t elemkind = 0;

                wasmsec.push_back(uint8_t(emode));
                switch (emode)
                {
                case 0:
                case 4:
                    wasmsec.append_expr_i32(edata->offset_expr);
                    break;

                case 2:
                    wasmsec.append_label_index(edata->tableidx);
                    wasmsec.append_expr_i32(edata->offset_expr);
                    fallthrough;
                case 1:
                case 3:
                    wasmsec.push_back(elemkind);
                    break;

                case 6:
                    wasmsec.append_label_index(edata->tableidx);
                    wasmsec.append_expr_i32(edata->offset_expr);
                    fallthrough;
                case 5:
                case 7:
                    wasmsec.append_leb128(int(edata->reftype));
                    break;

                default:
                    throw BUG("未知の Element 形式(", emode, ")です。");
                }

                uint32_t rtcnt = 0;

                for (ElementInit* init : *edata->init)
                    rtcnt += uint32_t(init->index.size()) + (init->code ? 1 : 0);
                wasmsec.append_leb128(rtcnt);
                for (ElementInit* init : *edata->init)
                {
                    if (!(emode & 4))
                    {
                        for (auto index : init->index)
                            wasmsec.append_label_index(index);
                        assert(!init->code);
                    }
                    else
                    {
                        for (auto index : init->index)
                        {
                            wasmsec.push_back(0xD2); // ref.func
                            wasmsec.append_label_index(index);
                            wasmsec.append_code_end();
                        }
                        if (init->code)
                            wasmsec.append_code(init->code);
                    }
                }
            }
            wasm.append_section(SectionID::ELEMENT, wasmsec);
        }
        put_custom_section(wasm, SectionID::ELEMENT);
    }

    void Parser::put_code_section(ModuleBinary& wasm)
    {
        FunctionSection* sec = module->function_section; assert(sec);
        size_t function_size = sec->size();

        if (function_size)
        {
            SectionBinary code;

            code.append_leb128(uint32_t(function_size));
            for (FunctionCode* fcode : *sec)
            {
                auto& locals = fcode->locals;
                SectionBinary func;

                func.append_leb128(uint32_t(locals.size()));
                for (auto& local : locals)
                {
                    func.append_leb128(uint32_t(local.size));
                    func.append_leb128(int(local.valtype));
                }
                func.append_code(fcode->code);

                code.append_leb128(uint32_t(func.size()));
                code.append_binary(func);
            }
            wasm.append_section(SectionID::CODE, code);
        }
        put_custom_section(wasm, SectionID::CODE);
    }

    void Parser::put_data_section(ModuleBinary& wasm)
    {
        DataSection* sec = module->data_section; assert(sec);
        size_t data_size = sec->size();

        if (data_size)
        {
            SectionBinary wasmsec;

            wasmsec.append_leb128(uint32_t(data_size));
            for (DataBlock* block : *sec)
            {
                if (!block->active)
                    wasmsec.append_leb128(uint32_t(1));
                else
                {
                    if (block->memidx <= 0)
                        wasmsec.append_leb128(uint32_t(0));
                    else
                    {
                        wasmsec.append_leb128(uint32_t(2));
                        wasmsec.append_leb128(uint32_t(block->memidx));
                    }
                    wasmsec.append_expr_i32(block->offset_expr);
                }

                SectionBinary data;

                for (CodeLine* line : *block->binary_code)
                    data.append_binary(line->binary);
                wasmsec.append_vector(data);
            }
            wasm.append_section(SectionID::DATA, wasmsec);
        }
        put_custom_section(wasm, SectionID::DATA);
    }

    void Parser::put_datacount_section(ModuleBinary& wasm)
    {
        DataSection* sec = module->data_section; assert(sec);
        size_t data_size = sec->size();

        if (data_size && option.section_datacount)
        {
            SectionBinary wasmsec;

            WASMGEN_DEBUG(2, "Data Count Section: ", data_size, "\n");

            wasmsec.append_leb128(uint32_t(data_size));
            wasm.append_section(SectionID::DATACOUNT, wasmsec);
        }
        put_custom_section(wasm, SectionID::DATACOUNT);
    }

    ////////////
    // Option //
    ////////////

    bool Parser::update_option(String* name, Token* tname, GetEqualRes& res,
                               Expression* op, String* optname, bool &option)
    {
        if (*name != *optname)
            return false;

        if (!res.value.isnumber())
            parse_error(ErrorCode::NOT_VALUE, {*(*op->children)[1]});
        else
        {
            option = bool(res.value);
            update_option(name, tname, option);
        }
        return true;
    }

    bool Parser::update_option(String* name, Token* tname, GetEqualRes& res,
                               Expression* op, String* optname,
                               int &option, int min, int max)
    {
        if (*name != *optname)
            return false;

        if (!res.value.isnumber())
            parse_error(ErrorCode::NOT_VALUE, {*(*op->children)[1]});
        else
        {
            int value = int(res.value);

            value = std::max<int>(min, value);
            value = std::min<int>(max, value);
            update_option(name, tname, option = value);
        }
        return true;
    }

    void Parser::update_option() noexcept
    {
        nestable_comment = option.comment_nest;
    }

    void Parser::Option::set_default() noexcept
    {
        include_depth = 10;
        comment_nest = false;
        paren_multiline = false;
        section_datacount = false;
        type_unique = false;
    }

} // wasmgen
