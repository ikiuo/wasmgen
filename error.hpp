/*
 * error.hpp
 */
#pragma once
#include "stdstr.hpp"

namespace wasmgen
{
    class BaseException : public StdString
    {
    private:
        using super = StdString;

    public:
        using super::super;
    };

    class Bug : public BaseException
    {
    private:
        using super = BaseException;

    public:
        using super::super;
    };

    class OSError : public BaseException
    {
    private:
        using super = BaseException;

    public:
        using super::super;
    };

#define BUG(...)  Bug(StringBlock{__FILE__, ":", __LINE__, ":BUG: ", {__VA_ARGS__}})

    /**/

    enum class ErrorCode
    {
        OK = 0,

        /**/

        _FILE = 1000,
        INTERNAL_ERROR,
        ASSEMBLY_FAILED,

        _FILE_INCLUDE = 1020,
        NO_INCLUDE_FILE,
        INCLUDE_NESTING_LIMIT,
        NO_FILE,

        _ALIAS_NAME = 1100,
        EXIST_ALIAS_NAME,

        _ASSEMBLE_SWITCH = 1120,
        NO_ASSEMBLY_SWITCH_CONDITION,
        TOO_MANY_ASSEMBLY_SWITCH_CONDITION,
        NO_ASSEMBLY_SWITCH_IF,
        INVALID_ASSEMBLY_SWITCH_ELIF,

        _MACRO_ = 1140,
        NO_MACRO_LABEL,
        EXIST_MACRO_NAME,
        NESTED_MACRO_DEFINITIONS,
        NO_MACRO_ARGUMENT_NAME,
        LABEL_REPLACEMENT_FAILED,
        NESTED_MACRO_EXPANSION,

        /**/

        _TOKEN = 1900,
        INVALID_ESCAPE_SEQUENCE,
        INVALID_OCTAL_CODE,
        INVALID_NUMBER_FORMAT,

        /**/

        _SYNTAX = 2000,
        SYNTAX_ERROR,
        DIVISION_BY_ZERO,
        INFINITE_LOOP,
        CANNOT_BE_INLINED,

        /**/

        _PARAMETER = 2020,
        NOT_VALUE,
        NOT_STRING,

        /**/

        _SECTION = 2100,
        NO_SECTION_LABEL,
        INVALID_SECTION_ID,
        INVALID_SECTION_NAME,
        EXIST_CODE_LABEL,
        NO_EXIST_IF,

        _CUSTOM_SECTION = 2120,
        NO_CUSTOM_SECTION_NAME,
        EXIST_CUSTOM_SECTION,

        _TYPE_SECTION = 2140,
        NO_TYPE_NAME,
        EXIST_TYPE_NAME,
        INVALID_TYPE_FORMAT,

        _IMPORT_SECTION = 2160,
        NO_MODULE_NAME,
        NO_IMPORT_NAME,

        _FUNCTION_SECTION = 2180,
        NO_FUNCTION_NAME,
        EXIST_FUNCTION_NAME,
        FUNCTION_FIXED_PARAMETERS,

        _TABLE_SECTION = 2200,
        NO_TABLE_NAME,
        EXIST_TABLE_NAME,

        _MEMORY_SECTION = 2220,
        NO_MEMORY_NAME,
        EXIST_MEMORY_NAME,

        _GLOBAL_SECTION = 2240,
        EXIST_GLOBAL_NAME,
        EXIST_GLOBAL_EXPR,

        _EXPORT_SECTION = 2260,
        NO_EXPORT_SYMBOL,
        NO_EXPORT_NAME,

        _START_SECTION = 2280,
        TOO_MANY_STARTS,

        _ELEMENT_SECTION = 2300,
        EXIST_ELEMENT_NAME,
        TOO_MANY_REFTYPE,
        TOO_MANY_TABLEIDX,
        TOO_MANY_OFFSET,
        NOT_ELEMENT_MDOE,
        NOT_ELEMENT_ACTIVE,
        INVALID_ELEMENT_CODE,
        INVALID_TABLEIDX,
        INVALID_OFFSET,

        _CODE_SECTION = 2320,

        _DATA_SECTION = 2340,
        EXIST_DATA_NAME,
        TOO_MANY_MEMIDX,
        NOT_DATA_ACTIVE,
        INVALID_DATA_CODE,

        _DATACOUNT_SECTION = 2360,

        _INSTRUCTION = 2400,
        INVALID_INSTRUCTION,
        TOO_FEW_OPERANDS,

        _OPERAND = 2420,
        NOT_REFTYPE,
        NOT_VALTYPE,
        NO_LIMITS,
        CAN_NOT_EQUAL,

        _PARAM = 2440,
        NO_PARAM_LABEL,
        EXIST_PARAM_NAME,

        _BINARY = 2460,
        OUT_OF_RANGE,

        /**/

        _WARNING = 3000,
        NESTED_COMMENTS,
        TOO_MANY_OPERANDS,
        IGNORE_CODE_AFTER_END,

        _WARNING_OPTION = 3020,
        IGNORE_UNKNOWN_OPTION,
        IGNORE_OPTION_LABEL,

        _WARNING_INCLUDE = 3040,
        IGNORE_INCLUDE_LABEL,

        _WARNING_VALUE = 3060,
        TRUNCATED_VALUE,

        _WARNING_CODE_LIST = 3080,
        NO_END,

        _WARNING_SECTION = 3100,
        IGNORE_CODE_LABEL,

        _WARNING_CUSTOM_SECTION = 3120,
        IGNORE_MESSAGE_LABEL,
        IGNORE_CUSTOM_SECTION_LABEL,

        _WARNING_EXPORT_SECTION = 3140,
        IGNORE_EXPORT_SECTION_LABEL,

        _WARNING_FUNCTYPE = 3160,
        IGNORE_FUNCTYPE_PARAMETER,

        _WARNING_ASSEMBLE_SWITCH = 3180,
        IGNORE_ASSEMBLE_SWITCH_LABEL,
        ASSEMBLE_SWITCH_NO_ENDIF,
        ASSEMBLE_SWITCH_NO_SET,
        ASSEMBLE_SWITCH_NO_UNSET,

        _WARNING_MACRO = 3200,
        IGNORE_MACRO_LABEL,
        NO_MACRO_END,
    };

    struct ErrorMessageData
    {
        StdVector<int> order;
        StdCStrList messages;
    };

    using ErrorCodeTable = StdMap<ErrorCode, ErrorMessageData>;

    extern ErrorCodeTable error_code_table;

} // wasmgen
