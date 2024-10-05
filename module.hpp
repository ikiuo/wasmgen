/*
 * module.hpp
 */
#pragma once
#include "section.hpp"

namespace wasmgen
{
    ////////////
    // Module //
    ////////////

    class Module : virtual public Object
    {
    public:
        SectionListPtr section[Section::size];

        CustomSectionPtr custom_section;
        TypeSectionPtr type_section;
        ImportSectionPtr import_section;
        FunctionSectionPtr function_section;
        TableSectionPtr table_section;
        MemorySectionPtr memory_section;
        GlobalSectionPtr global_section;
        ExportSectionPtr export_section;
        StartSectionPtr start_section;
        ElementSectionPtr element_section;
        DataSectionPtr data_section;

        IdentifierPtr typeidx;
        IdentifierPtr funcidx;
        IdentifierPtr tableidx;
        IdentifierPtr memidx;
        IdentifierPtr globalidx;
        IdentifierPtr elemidx;
        IdentifierPtr dataidx;

    public:
        Module();
    };

    using ModulePtr = Pointer<Module>;
    using ModuleRef = RefPointer<Module>;
    using NewModule = NewPointer<Module>;

    //////////////////
    // ModuleBinary //
    //////////////////

    class ModuleBinary : public ByteArray
    {
    private:
        using super = ByteArray;

    public:
        using super::super;

        void append_section(SectionID id, const ByteArray& data);
        void append_functype(FuncType& restype);
        void append_resulttype(ResultType& restype);
        void append_valtype(ValType valtype);
        void append_limits(Limits& limits);
        void append_label_index(int index);
        void append_expr_i32(CodeList* list, uint32_t value = 0);
        void append_code(CodeList* list);
        void append_code_i32_const(uint32_t value = 0);
        void append_code_end(size_t count = 1);
    };

    ///////////////////
    // SectionBinary //
    ///////////////////

    using SectionBinary = ModuleBinary;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    //////////////////
    // ModuleBinary //
    //////////////////

    inline void ModuleBinary::append_valtype(ValType valtype)
    {
        append_leb128(int8_t(valtype));
    }

    inline void ModuleBinary::append_label_index(int index)
    {
        assert(index >= 0);
        append_leb128(uint32_t(index));
    }

    inline void ModuleBinary::append_expr_i32(CodeList* list, uint32_t value)
    {
        if (!list)
            append_code_i32_const(value);
        else
            append_code(list);
    }

    inline void ModuleBinary::append_code_end(size_t count)
    {
        insert(end(), count, 0x0B);
    }

} // wasmgen
