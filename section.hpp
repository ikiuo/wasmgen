/*
 * section.hpp
 */
#pragma once
#include "data.hpp"
#include "instruction.hpp"
#include "label.hpp"

namespace wasmgen
{

    class MacroData;
    using MacroDataPtr = Pointer<MacroData>;
    using MacroDataRef = RefPointer<MacroData>;
    using NewMacroData = NewPointer<MacroData>;

    ///////////////
    // SectionID //
    ///////////////

    enum class SectionID
    {
        CUSTOM,    // 0
        TYPE,      // 1
        IMPORT,    // 2
        FUNCTION,  // 3
        TABLE,     // 4
        MEMORY,    // 5
        GLOBAL,    // 6
        EXPORT,    // 7
        START,     // 8
        ELEMENT,   // 9
        CODE,      // 10
        DATA,      // 11
        DATACOUNT, // 12

        SIZE,      // 13

        UNDEFINED = -1,
    };

    //////////////
    // CodeLine //
    //////////////

    class CodeLine : virtual public Object
    {
    public:
        TokenPtr label;
        TokenPtr instr;
        InstrIter instab;
        MacroDataPtr macro;
        ExpressionListPtr operands;
        int operand_start;
        ByteArray binary;

    public:
        CodeLine();
        CodeLine(ExpressionList* ops, int opstart = 0);

        Expression* getop(size_t n) noexcept;
    };

    using CodeLinePtr = Pointer<CodeLine>;
    using CodeLineRef = RefPointer<CodeLine>;
    using NewCodeLine = NewPointer<CodeLine>;

    //////////////////
    // CodeLineList //
    //////////////////

    using CodeLineList = PointerArray<CodeLine>;

    using CodeLineListPtr = Pointer<CodeLineList>;
    using CodeLineListRef = RefPointer<CodeLineList>;
    using NewCodeLineList = NewPointer<CodeLineList>;

    //////////////
    // CodeList //
    //////////////

    class CodeList : public CodeLineList
    {
    private:
        using super = PointerArray<CodeLine>;

    public:
        IdentifierPtr label;
        IdentifierPtr br_idx;
        IdentifierListPtr symbol;

        bool binary;
        bool retry;
        int pass;
        size_t offset;
        CodeLinePtr code_end;
        ObjectStack<CodeLine> block_stack;

    public:
        CodeList();
        template <typename... Args> CodeList(Args... args);
    };

    using CodeListPtr = Pointer<CodeList>;
    using CodeListPef = RefPointer<CodeList>;
    using NewCodeList = NewPointer<CodeList>;

    ///////////////
    // CodeBlock //
    ///////////////

    class CodeBlock;

    using CodeBlockPtr = Pointer<CodeBlock>;
    using CodeBlockPef = RefPointer<CodeBlock>;
    using NewCodeBlock = NewPointer<CodeBlock>;

    class CodeBlock : public CodeList
    {
    private:
        using super = CodeList;

    public:
        CodeBlockPtr block;

    public:
        using super::super;

        bool isblock() const noexcept;
    };

    ///////////////////
    // CodeBlockList //
    ///////////////////

    using CodeBlockList = PointerArray<CodeBlock>;

    using CodeBlockListPtr = Pointer<CodeBlockList>;
    using CodeBlockListPef = RefPointer<CodeBlockList>;
    using NewCodeBlockList = NewPointer<CodeBlockList>;

    ///////////////
    // MacroData //
    ///////////////

    class MacroData : virtual public Object
    {
    public:
        CodeLinePtr line;
        TokenListList code;

    public:
        MacroData(CodeLine* line = nullptr);
    };

    /////////////
    // Section //
    /////////////

    class Section : virtual public Object
    {
    public:
        SectionID id;
        CodeListPtr code;

        static constexpr size_t size = size_t(SectionID::SIZE);

    public:
        Section(SectionID id) noexcept;
    };

    using SectionPtr = Pointer<Section>;
    using SectionRef = RefPointer<Section>;
    using NewSection = NewPointer<Section>;

    /////////////////
    // SectionList //
    /////////////////

    using SectionList = PointerArray<Section>;

    using SectionListPtr = Pointer<SectionList>;
    using SectionListPef = RefPointer<SectionList>;
    using NewSectionList = NewPointer<SectionList>;

    /*
     *
     */

    ///////////////////////
    // CustomSectionData //
    ///////////////////////

    class CustomSectionData : virtual public Object
    {
    public:
        SectionID id;
        StringPtr name;
        TokenPtr label;
        CodeListPtr code;

    public:
        CustomSectionData() noexcept;
    };

    using CustomSectionDataPtr = Pointer<CustomSectionData>;
    using CustomSectionDataRef = RefPointer<CustomSectionData>;
    using NewCustomSectionData = NewPointer<CustomSectionData>;

    ///////////////////
    // CustomSection //
    ///////////////////

    class CustomSection : virtual public Object
    {
    public:
        CustomSectionDataPtr section[Section::size];

    public:
        CustomSection();
    };

    using CustomSectionPtr = Pointer<CustomSection>;
    using CustomSectionRef = RefPointer<CustomSection>;
    using NewCustomSection = NewPointer<CustomSection>;

    /*
     *
     */

    //////////////
    // TypeData //
    //////////////

    class TypeData : virtual public Object
    {
    public:
        StringPtr name;
        FuncType type;
        IdentifierPtr arg;
        int index;

    public:
        TypeData();

        operator FuncType&() noexcept;
    };

    using TypeDataPtr = Pointer<TypeData>;
    using TypeDataRef = RefPointer<TypeData>;
    using NewTypeData = NewPointer<TypeData>;

    //////////////////
    // TypeDataList //
    //////////////////

    using TypeDataList = PointerArray<TypeData>;

    using TypeDataListPtr = Pointer<TypeDataList>;
    using TypeDataListRef = RefPointer<TypeDataList>;
    using NewTypeDataList = NewPointer<TypeDataList>;

    /////////////////
    // TypeDataMap //
    /////////////////

    using TypeDataMap = StdStringMap<TypeDataPtr>;

    /////////////////
    // TypeSection //
    /////////////////

    class TypeSection : public TypeDataList
    {
    public:
        FuncTypeList type;
        FuncTypeList btype;
        FuncTypeMap index;
        FuncTypeMap bindex;
        TypeDataMap name;
    };

    using TypeSectionPtr = Pointer<TypeSection>;
    using TypeSectionRef = RefPointer<TypeSection>;
    using NewTypeSection = NewPointer<TypeSection>;

    /*
     *
     */

    ////////////////
    // ImportData //
    ////////////////

    class ImportData : virtual public Object
    {
    public:
        enum Desc
        {
            NONE = -1,
            FUNCTION,
            TABLE,
            MEMORY,
            GLOBAL,
        };

        Desc desc;
        StringPtr label;
        StringPtr mod;
        StringPtr name;
        int typeidx;
        ValType valtype;
        int mut;
        Limits limits;
        CodeLinePtr code;
        int index;

    public:
        ImportData();
    };

    using ImportDataPtr = Pointer<ImportData>;
    using ImportDataRef = RefPointer<ImportData>;
    using NewImportData = NewPointer<ImportData>;

    ////////////////////
    // ImportDataList //
    ////////////////////

    using ImportDataList = PointerArray<ImportData>;

    using ImportDataListPtr = Pointer<ImportDataList>;
    using ImportDataListRef = RefPointer<ImportDataList>;
    using NewImportDataList = NewPointer<ImportDataList>;

    ///////////////////
    // ImportSection //
    ///////////////////

    using ImportSection = ImportDataList;

    using ImportSectionPtr = Pointer<ImportSection>;
    using ImportSectionRef = RefPointer<ImportSection>;
    using NewImportSection = NewPointer<ImportSection>;

    /*
     *
     */

    //////////////////
    // FunctionCode //
    //////////////////

    class FunctionCode : virtual public Object
    {
    public:
        struct Locals
        {
            size_t size;
            ValType valtype;
        };

        using LocalsList = StdVector<Locals>;

    public:
        StringPtr name;
        TypeDataPtr type;
        LocalsList locals;
        IdentifierPtr local;
        CodeListPtr code;
        int index;

    public:
        FunctionCode();
    };

    using FunctionCodePtr = Pointer<FunctionCode>;
    using FunctionCodeRef = RefPointer<FunctionCode>;
    using NewFunctionCode = NewPointer<FunctionCode>;

    //////////////////////
    // FunctionCodeList //
    //////////////////////

    using FunctionCodeList = PointerArray<FunctionCode>;

    using FunctionCodeListPtr = Pointer<FunctionCodeList>;
    using FunctionCodeListRef = RefPointer<FunctionCodeList>;
    using NewFunctionCodeList = NewPointer<FunctionCodeList>;

    /////////////////////
    // FunctionCodeMap //
    /////////////////////

    using FunctionCodeMap = StdStringMap<FunctionCodePtr>;

    /////////////////////
    // FunctionSection //
    /////////////////////

    class FunctionSection : public FunctionCodeList
    {
    public:
        FunctionCodeMap name;
    };

    using FunctionSectionPtr = Pointer<FunctionSection>;
    using FunctionSectionRef = RefPointer<FunctionSection>;
    using NewFunctionSection = NewPointer<FunctionSection>;

    /*
     *
     */

    ///////////////
    // TableData //
    ///////////////

    class TableData : virtual public Object
    {
    public:
        StringPtr name;
        RefType reftype;
        Limits limits;
        int index;

    public:
        TableData();
    };

    using TableDataPtr = Pointer<TableData>;
    using TableDataRef = RefPointer<TableData>;
    using NewTableData = NewPointer<TableData>;

    ///////////////////
    // TableDataList //
    ///////////////////

    using TableDataList = PointerArray<TableData>;

    using TableDataListPtr = Pointer<TableDataList>;
    using TableDataListRef = RefPointer<TableDataList>;
    using NewTableDataList = NewPointer<TableDataList>;

    //////////////////
    // TableDataMap //
    //////////////////

    using TableDataMap = StdStringMap<TableDataPtr>;

    //////////////////
    // TableSection //
    //////////////////

    using TableSection = TableDataList;

    using TableSectionPtr = Pointer<TableSection>;
    using TableSectionRef = RefPointer<TableSection>;
    using NewTableSection = NewPointer<TableSection>;

    /*
     *
     */

    ////////////////
    // MemoryData //
    ////////////////

    class MemoryData : virtual public Object
    {
    public:
        StringPtr name;
        Limits limits;
        int index;

    public:
        MemoryData();
    };

    using MemoryDataPtr = Pointer<MemoryData>;
    using MemoryDataRef = RefPointer<MemoryData>;
    using NewMemoryData = NewPointer<MemoryData>;

    ////////////////////
    // MemoryDataList //
    ////////////////////

    using MemoryDataList = PointerArray<MemoryData>;

    using MemoryDataListPtr = Pointer<MemoryDataList>;
    using MemoryDataListRef = RefPointer<MemoryDataList>;
    using NewMemoryDataList = NewPointer<MemoryDataList>;

    ///////////////////
    // MemoryDataMap //
    ///////////////////

    using MemoryDataMap = StdStringMap<MemoryDataPtr>;

    ///////////////////
    // MemorySection //
    ///////////////////

    using MemorySection = MemoryDataList;

    using MemorySectionPtr = Pointer<MemorySection>;
    using MemorySectionRef = RefPointer<MemorySection>;
    using NewMemorySection = NewPointer<MemorySection>;

    /*
     *
     */

    ////////////////
    // GlobalData //
    ////////////////

    class GlobalData : virtual public Object
    {
    public:
        StringPtr name;
        bool mut;
        ValType valtype;
        CodeListPtr code;
        int index;

    public:
        GlobalData();
    };

    using GlobalDataPtr = Pointer<GlobalData>;
    using GlobalDataRef = RefPointer<GlobalData>;
    using NewGlobalData = NewPointer<GlobalData>;

    ////////////////////
    // GlobalDataList //
    ////////////////////

    using GlobalDataList = PointerArray<GlobalData>;

    using GlobalDataListPtr = Pointer<GlobalDataList>;
    using GlobalDataListRef = RefPointer<GlobalDataList>;
    using NewGlobalDataList = NewPointer<GlobalDataList>;

    ///////////////////
    // GlobalSection //
    ///////////////////

    using GlobalSection = GlobalDataList;

    using GlobalSectionPtr = Pointer<GlobalSection>;
    using GlobalSectionRef = RefPointer<GlobalSection>;
    using NewGlobalSection = NewPointer<GlobalSection>;

    /*
     *
     */

    ////////////////
    // ExportData //
    ////////////////

    class ExportData : virtual public Object
    {
    public:
        enum Desc
        {
            NONE = -1,
            FUNCTION,
            TABLE,
            MEMORY,
            GLOBAL,
        };

        Desc desc;
        StringPtr name;
        int index;

    public:
        ExportData();
    };

    using ExportDataPtr = Pointer<ExportData>;
    using ExportDataRef = RefPointer<ExportData>;
    using NewExportData = NewPointer<ExportData>;

    ////////////////////
    // ExportDataList //
    ////////////////////

    using ExportDataList = PointerArray<ExportData>;

    using ExportDataListPtr = Pointer<ExportDataList>;
    using ExportDataListRef = RefPointer<ExportDataList>;
    using NewExportDataList = NewPointer<ExportDataList>;

    ///////////////////
    // ExportSection //
    ///////////////////

    using ExportSection = ExportDataList;

    using ExportSectionPtr = Pointer<ExportSection>;
    using ExportSectionRef = RefPointer<ExportSection>;
    using NewExportSection = NewPointer<ExportSection>;

    /*
     *
     */

    //////////////////
    // StartSection //
    //////////////////

    class StartSection : virtual public Object
    {
    public:
        int funcidx;

    public:
        StartSection();
    };

    using StartSectionPtr = Pointer<StartSection>;
    using StartSectionRef = RefPointer<StartSection>;
    using NewStartSection = NewPointer<StartSection>;

    /*
     *
     */

    /////////////////
    // ElementMode //
    /////////////////

    enum class ElementMode
    {
        NONE,
        ACTIVE,
        DECLARATIVE,
        PASSIVE,
    };

    /////////////////
    // ElementInit //
    /////////////////

    class ElementInit : virtual public Object
    {
    public:
        FuncIdxList index;
        CodeListPtr code;

    public:
        ElementInit();

        bool empty() const noexcept;
    };

    using ElementInitPtr = Pointer<ElementInit>;
    using ElementInitRef = RefPointer<ElementInit>;
    using NewElementInit = NewPointer<ElementInit>;

    /////////////////////
    // ElementInitList //
    /////////////////////

    using ElementInitList = PointerArray<ElementInit>;

    using ElementInitListPtr = Pointer<ElementInitList>;
    using ElementInitListRef = RefPointer<ElementInitList>;
    using NewElementInitList = NewPointer<ElementInitList>;

    /////////////////
    // ElementData //
    /////////////////

    class ElementData : virtual public Object
    {
    public:
        StringPtr name;
        RefType reftype;
        ElementMode mode;
        int tableidx;
        CodeListPtr offset_expr;
        bool funcidx_expr;
        ElementInitListPtr init;
        int index;

    public:
        ElementData();
    };

    using ElementDataPtr = Pointer<ElementData>;
    using ElementDataRef = RefPointer<ElementData>;
    using NewElementData = NewPointer<ElementData>;

    /////////////////////
    // ElementDataList //
    /////////////////////

    using ElementDataList = PointerArray<ElementData>;

    using ElementDataListPtr = Pointer<ElementDataList>;
    using ElementDataListRef = RefPointer<ElementDataList>;
    using NewElementDataList = NewPointer<ElementDataList>;

    ////////////////////
    // ElementSection //
    ////////////////////

    using ElementSection = ElementDataList;

    using ElementSectionPtr = Pointer<ElementSection>;
    using ElementSectionRef = RefPointer<ElementSection>;
    using NewElementSection = NewPointer<ElementSection>;

    /*
     *
     */

    ///////////////
    // DataBlock //
    ///////////////

    class DataBlock : virtual public Object
    {
    public:
        StringPtr name;
        bool active;
        int memidx;
        CodeListPtr offset_expr;
        CodeListPtr binary_code;
        int index;

    public:
        DataBlock();
    };

    using DataBlockPtr = Pointer<DataBlock>;
    using DataBlockRef = RefPointer<DataBlock>;
    using NewDataBlock = NewPointer<DataBlock>;

    ///////////////////
    // DataBlockList //
    ///////////////////

    using DataBlockList = PointerArray<DataBlock>;

    using DataBlockListPtr = Pointer<DataBlockList>;
    using DataBlockListRef = RefPointer<DataBlockList>;
    using NewDataBlockList = NewPointer<DataBlockList>;

    /////////////////
    // DataSection //
    /////////////////

    using DataSection = DataBlockList;

    using DataSectionPtr = Pointer<DataSection>;
    using DataSectionRef = RefPointer<DataSection>;
    using NewDataSection = NewPointer<DataSection>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    //////////////
    // TypeData //
    //////////////

    inline TypeData::operator FuncType&() noexcept
    {
        return type;
    }

    //////////////
    // CodeLine //
    //////////////

    inline Expression* CodeLine::getop(size_t n) noexcept
    {
        return operands && n < operands->size() ? &(*operands)[n] : nullptr;
    }

    //////////////
    // CodeList //
    //////////////

    template <typename... Args>
    CodeList::CodeList(Args... args)
        : super(args...)
        , binary(false)
        , retry(false)
        , pass(0)
        , offset(0)
    {
        /*NOOP*/
    }

    ///////////////
    // CodeBlock //
    ///////////////

    inline bool CodeBlock::isblock() const noexcept
    {
        return !size();
    }

    /////////////////
    // ElementInit //
    /////////////////

    inline bool ElementInit::empty() const noexcept
    {
        return !index.size() && !code;
    }

} // wasmgen
