/*
 * section.cpp
 */
#include "section.hpp"

namespace wasmgen
{
    //////////////
    // CodeLine //
    //////////////

    CodeLine::CodeLine()
        : CodeLine(new ExpressionList)
    {
        /*NOOP*/
    }

    CodeLine::CodeLine(ExpressionList* ops, int opstart)
        : operands(ops)
        , operand_start(opstart)
    {
        /*NOOP*/
    }

    //////////////
    // CodeList //
    //////////////

    CodeList::CodeList()
        : binary(false)
        , retry(false)
        , pass(0)
        , offset(0)
    {
        /*NOOP*/
    }

    /////////////
    // Section //
    /////////////

    Section::Section(SectionID id) noexcept
        : id(id)
        , code(new CodeList)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ///////////////////////
    // CustomSectionData //
    ///////////////////////

    CustomSectionData::CustomSectionData() noexcept
        : id(SectionID::UNDEFINED)
    {
        /*NOOP*/
    }

    ///////////////////
    // CustomSection //
    ///////////////////

    CustomSection::CustomSection()
    {
        /*NOOP*/
    }

    /*
     *
     */

    //////////////
    // TypeData //
    //////////////

    TypeData::TypeData()
        : arg(new Identifier)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ////////////////
    // ImportData //
    ////////////////

    ImportData::ImportData()
        : desc(NONE)
        , typeidx(-1)
        , valtype(ValType::NONE)
        , mut(0)
        , limits(0)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    //////////////////
    // FunctionCode //
    //////////////////

    FunctionCode::FunctionCode()
        : code(new CodeList)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ///////////////
    // TableData //
    ///////////////

    TableData::TableData()
        : reftype(RefType::NONE)
        , limits(0)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ////////////////
    // MemoryData //
    ////////////////

    MemoryData::MemoryData()
        : limits(0)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ////////////////
    // GlobalData //
    ////////////////

    GlobalData::GlobalData()
        : valtype(ValType::NONE)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    ////////////////
    // ExportData //
    ////////////////

    ExportData::ExportData()
        : desc(NONE)
        , index(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    //////////////////
    // StartSection //
    //////////////////

    StartSection::StartSection()
        : funcidx(-1)
    {
        /*NOOP*/
    }

    /*
     *
     */

    /////////////////
    // ElementInit //
    /////////////////

    ElementInit::ElementInit()
    {
        /*NOOP*/
    }

    /////////////////
    // ElementData //
    /////////////////

    ElementData::ElementData()
        : reftype(RefType::NONE)
        , mode(ElementMode::NONE)
        , tableidx(-1)
        , offset_expr()
        , funcidx_expr(false)
        , init(new ElementInitList)
        , index(-1)
    {
        /*NOOP*/
    }

    ///////////////
    // DataBlock //
    ///////////////

    DataBlock::DataBlock()
        : active()
        , memidx(-1)
        , offset_expr()
        , binary_code(new CodeList)
        , index(-1)
    {
        /*NOOP*/
    }

} // wasmgen
