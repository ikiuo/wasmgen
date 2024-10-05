/*
 * module.cpp
 */
#include "module.hpp"

namespace wasmgen
{
    ////////////
    // Module //
    ////////////

    Module::Module()
        : custom_section(new CustomSection)
        , type_section(new TypeSection)
        , import_section(new ImportSection)
        , function_section(new FunctionSection)
        , table_section(new TableSection)
        , memory_section(new MemorySection)
        , global_section(new GlobalSection)
        , export_section(new ExportSection)
        , start_section(new StartSection)
        , element_section(new ElementSection)
        , data_section(new DataSection)
          /**/
        , typeidx(new Identifier)
        , funcidx(new Identifier)
        , tableidx(new Identifier)
        , memidx(new Identifier)
        , globalidx(new Identifier)
        , elemidx(new Identifier)
        , dataidx(new Identifier)
    {
        for (auto i : inc_range<size_t>(Section::size))
            section[i] = new SectionList;
    }

    //////////////////
    // ModuleBinary //
    //////////////////

    void ModuleBinary::append_section(SectionID id, const ByteArray& data)
    {
        if (data.size())
        {
            push_back(uint8_t(id));
            append_vector(data);
        }
    }

    void ModuleBinary::append_functype(FuncType& functype)
    {
        push_back(0x60);
        append_resulttype(functype.arg);
        append_resulttype(functype.res);
    }

    void ModuleBinary::append_resulttype(ResultType& restype)
    {
        append_leb128(uint32_t(restype.size()));
        for (auto& rt : restype)
            append_leb128(int8_t(rt));
    }

    void ModuleBinary::append_limits(Limits& limits)
    {
        push_back(uint8_t(limits.mode));
        append_leb128(limits.min);
        if (limits.mode)
            append_leb128(limits.max);
    }

    void ModuleBinary::append_code(CodeList* list)
    {
        assert(list);
        for (CodeLine* line : *list)
        {
            assert(line);
            append_binary(line->binary);
        }
        if (!list->code_end && !list->binary)
            append_code_end(list->block_stack.size() + 1);
    }

    void ModuleBinary::append_code_i32_const(uint32_t value)
    {
        push_back(0x41); // i32.const
        append_leb128(value);
        append_code_end();
    }

} // wasmgen
