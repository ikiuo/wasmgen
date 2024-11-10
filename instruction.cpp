/*
 * instruction.cpp
 */
#include "instruction.hpp"

namespace wasmgen
{
    namespace Instruction
    {
        // --------------------------------------------------------------------

        const Data table {
#include "instruction_table.hpp"
            /*
             *
             */
            {
                "", // empty
                {
                    Operation(PSEUDO_EMPTY),
                    Operand(0),
                    0, { 0 }
                }
            },
            /*
             *
             */
            {
                "end.block",
                {
                    Operation(ST_LEAVE_BLOCK),
                    Operand(0),
                    1, { 11 }
                }
            },
            {
                "end.loop",
                {
                    Operation(ST_LEAVE_LOOP),
                    Operand(0),
                    1, { 11 }
                }
            },
            {
                "end.if",
                {
                    Operation(ST_LEAVE_IF),
                    Operand(0),
                    1, { 11 }
                }
            },
            /*
             *
             */
#define PSEUDO(name, code)                      \
            {                                   \
                name,                           \
                {                               \
                    Operation(code),            \
                    Operand(0),                 \
                    0, { 0 }                    \
                }                               \
            }

            PSEUDO(".align",   BD_ALIGN),
            PSEUDO(".balign",  BD_BALIGN),
            PSEUDO(".i8",      BD_I8),
            PSEUDO(".i8le",    BD_I8),
            PSEUDO(".i8be",    BD_I8_BE),
            PSEUDO(".i16",     BD_I16),
            PSEUDO(".i16le",   BD_I16),
            PSEUDO(".i16be",   BD_I16_BE),
            PSEUDO(".i32",     BD_I32),
            PSEUDO(".i32le",   BD_I32),
            PSEUDO(".i32be",   BD_I32_BE),
            PSEUDO(".i64",     BD_I64),
            PSEUDO(".i64le",   BD_I64),
            PSEUDO(".i64be",   BD_I64_BE),
            PSEUDO(".f32",     BD_F32),
            PSEUDO(".f32le",   BD_F32),
            PSEUDO(".f32be",   BD_F32_BE),
            PSEUDO(".f64",     BD_F64),
            PSEUDO(".f64le",   BD_F64),
            PSEUDO(".f64be",   BD_F64_BE),
            PSEUDO(".sleb",    BD_SLEB),
            PSEUDO(".uleb",    BD_ULEB),
            PSEUDO(".string",  BD_STRING),
            PSEUDO(".nstring", BD_NSTRING),
            /*-*/
            PSEUDO(".custom", PSEUDO_CUSTOM),
            PSEUDO(".type",   PSEUDO_TYPE),
            /**/
            PSEUDO(".import", PSEUDO_IMPORT),
            PSEUDO(".import.func",   IMP_FUNC),
            PSEUDO(".import.table",  IMP_TABLE),
            PSEUDO(".import.memory", IMP_MEMORY),
            PSEUDO(".import.const",  IMP_CONST),
            PSEUDO(".import.var",    IMP_VAR),
            PSEUDO(".imp.func",   IPC_FUNC),
            PSEUDO(".imp.table",  IPC_TABLE),
            PSEUDO(".imp.memory", IPC_MEMORY),
            PSEUDO(".imp.const",  IPC_CONST),
            PSEUDO(".imp.var",    IPC_VAR),
            /**/
            PSEUDO(".code",   PSEUDO_FUNCODE),
            // PSEUDO(".func",   PSEUDO_FUNCODE),
            PSEUDO(".table",  PSEUDO_TABLE),
            PSEUDO(".memory", PSEUDO_MEMORY),
            /**/
            PSEUDO(".gconst", PG_CONST),
            PSEUDO(".gvar",   PG_VAR),
            /**/
            PSEUDO(".export",    PSEUDO_EXPORT),
            PSEUDO(".export.func",   PSEUDO_EXPORT_FUNC),
            PSEUDO(".export.table",  PSEUDO_EXPORT_TABLE),
            PSEUDO(".export.memory", PSEUDO_EXPORT_MEMORY),
            PSEUDO(".export.global", PSEUDO_EXPORT_GLOBAL),
            PSEUDO(".start",     PSEUDO_START),
            PSEUDO(".element",   PSEUDO_ELEMENT),
            PSEUDO(".data",      PSEUDO_DATA),
            // PSEUDO(".datacount", PSEUDO_DATACOUNT),
            /*-*/
            PSEUDO(".impfunc",   IPC_FUNC),
            PSEUDO(".imptable",  IPC_TABLE),
            PSEUDO(".impmemory", IPC_MEMORY),
            PSEUDO(".impconst",  IPC_CONST),
            PSEUDO(".impvar",    IPC_VAR),
            /**/
            PSEUDO(".param", FPC_PARAM),
            PSEUDO(".local", FPC_LOCAL),
            /**/
            PSEUDO(".reftype",  EPC_REFTYPE),
            PSEUDO(".tableidx", EPC_TABLEIDX),
            PSEUDO(".offset",   EPC_OFFSET),
            PSEUDO(".funcidx",  EPC_FUNCIDX),
            /**/
            PSEUDO(".memidx", DPC_MEMIDX),
            /*-*/
            PSEUDO(".defstruct", PSEUDO_STRUCT_BEGIN),
            PSEUDO(".endstruct", PSEUDO_STRUCT_END),
            PSEUDO(".member", PSEUDO_STRUCT_MEMBER),
            /*-*/
            PSEUDO(".defmacro", PSEUDO_MACRO_BEGIN),
            PSEUDO(".endmacro", PSEUDO_MACRO_END),
            PSEUDO(".delmacro", PSEUDO_MACRO_DELETE),
            /*-*/
            PSEUDO(".alias", PSEUDO_ALIAS),
            /*-*/
            PSEUDO("@if",    ASW_IF),
            PSEUDO("@elif",  ASW_ELIF),
            PSEUDO("@else",  ASW_ELSE),
            PSEUDO("@endif", ASW_ENDIF),
            PSEUDO("@set",   ASW_SET),
            PSEUDO("@unset", ASW_UNSET),
            /**/
            PSEUDO("@message", PSEUDO_MESSAGE),
            PSEUDO("@include", PSEUDO_INCLUDE),
            PSEUDO("@option",  PSEUDO_OPTION),

#undef PSEUDO
        };

        // --------------------------------------------------------------------

    } // Instruction
} // wasmgen
