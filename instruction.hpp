/*
 * instruction.hpp
 */
#pragma once
#include "utility.hpp"

namespace wasmgen
{
    namespace Instruction
    {
        enum DataType
        {
            DT_NONE,  // 0
            DT_I32,   // 1
            DT_I64,   // 2
            DT_F32,   // 3
            DT_F64,   // 4
            DT_V128,  // 5
            DT_FUNC,  // 6
            DT_ANY,   // 7
        };

        enum OperandCode
        {
            OP_NONE,  // 0

            OP_I32,   // 1
            OP_I64,   // 2

            OP_F32,   // 3
            OP_F64,   // 4

            OP_V128,  // 5
            OP_VI08 = OP_V128,
            OP_VI16,  // 6
            OP_VI32,  // 7
            OP_VI64,  // 8
            OP_VF32,  // 9
            OP_VF64,  // 10

            OP_VL,    // 11
            OP_VLT,   // 12

            OP_DID,   // 13
            OP_EID,   // 14
            OP_FID,   // 15
            OP_GID,   // 16
            OP_LID,   // 17
            OP_MID,   // 18
            OP_SID,   // 19
            OP_SIDn,  // 20
            OP_TID,   // 21
            OP_TID1,  // 22
            OP_TID2,  // 23
            OP_XID,   // 24

            OP_REF,   // 25
            OP_VTn,   // 26
            OP_BT,    // 27

            OP_MA0,   // 28
            OP_MA1,   // 29
            OP_MA2,   // 30
            OP_MA3,   // 31
            OP_MA4,   // 32
        };

        enum StackMode
        {
            ST_NONE,
            ST_LEAVE,
            ST_ENTER,
            ST_ELSE,
        };

        enum MacroCode
        {
            MC_NONE,

            MC_COND_BR,
        };

        enum BinaryCode
        {
            BD_NONE,

            BD_ALIGN,
            BD_BALIGN,

            BD_I8,
            BD_I8_BE,
            BD_I16,
            BD_I16_BE,
            BD_I32,
            BD_I32_BE,
            BD_I64,
            BD_I64_BE,
            BD_F32,
            BD_F32_BE,
            BD_F64,
            BD_F64_BE,

            BD_SLEB,
            BD_ULEB,

            BD_STRING,
            BD_NSTRING,
        };

        enum ImportCode
        {
            IMP_NONE,

            IMP_FUNC,
            IMP_TABLE,
            IMP_MEMORY,
            IMP_CONST,
            IMP_VAR,
        };

        enum ImportParamCode
        {
            IPC_NONE   = IMP_NONE,

            IPC_FUNC   = IMP_FUNC,
            IPC_TABLE  = IMP_TABLE,
            IPC_MEMORY = IMP_MEMORY,
            IPC_CONST  = IMP_CONST,
            IPC_VAR    = IMP_VAR,
        };

        enum GlobalCode
        {
            PG_NONE,

            PG_CONST,
            PG_VAR,
        };

        enum FuncParamCode
        {
            FPC_NONE,

            FPC_PARAM,
            FPC_LOCAL,
        };

        enum ElemParamCode
        {
            EPC_NONE,

            EPC_REFTYPE,
            EPC_TABLEIDX,
            EPC_OFFSET,
            EPC_FUNCIDX,
        };

        enum DataParamCode
        {
            DPC_NONE,

            DPC_MEMIDX = EPC_TABLEIDX,
            DPC_OFFSET = EPC_OFFSET,
        };

        enum AssembeSwitch
        {
            ASW_NONE,

            ASW_IF,
            ASW_ELIF,
            ASW_ELSE,
            ASW_ENDIF,
            ASW_SET,
            ASW_UNSET,
        };

        enum PseudoCode
        {
            INSTRUCTION,
            INSTR_MACRO,

            PSEUDO_ASSMBLE_SWITCH,
            PSEUDO_INCLUDE,
            PSEUDO_MESSAGE,
            PSEUDO_OPTION,

            PSEUDO_ALIAS,

            PSEUDO_EMPTY,
            PSEUDO_BINARY,

            PSEUDO_CUSTOM,
            PSEUDO_TYPE,
            PSEUDO_IMPORT,
            PSEUDO_FUNCODE,
            PSEUDO_TABLE,
            PSEUDO_MEMORY,
            PSEUDO_GLOBAL,
            PSEUDO_EXPORT,
            PSEUDO_EXPORT_FUNC,
            PSEUDO_EXPORT_TABLE,
            PSEUDO_EXPORT_MEMORY,
            PSEUDO_EXPORT_GLOBAL,
            PSEUDO_START,
            PSEUDO_ELEMENT,
            PSEUDO_DATA,
            PSEUDO_DATACOUNT,

            /**/

            PSEUDO_IMPORT_PARAM,
            PSEUDO_FUNC_PARAM,
            PSEUDO_ELEM_PARAM,
            PSEUDO_DATA_PARAM,

            /**/

            PSEUDO_SIZE,
        };

        ///////////////
        // Operation //
        ///////////////

        struct Operation
        {
            enum Constant
            {
                PSEUDO_CODE_BIT = 24,
                PSEUDO_CODE_MASK = (1 << PSEUDO_CODE_BIT) - 1,
            };

            uint32_t value;

            Operation() noexcept = default;
            constexpr Operation(const Operation&) noexcept = default;
            constexpr Operation(int n, int p1, int p2, int p3, int res, int stack) noexcept;
            constexpr Operation(uint32_t value) noexcept;
            constexpr Operation(MacroCode pcode) noexcept;
            constexpr Operation(PseudoCode pcode) noexcept;
            constexpr Operation(BinaryCode bcode) noexcept;
            constexpr Operation(ImportCode icode) noexcept;
            constexpr Operation(GlobalCode gcode) noexcept;
            constexpr Operation(ImportParamCode fcode) noexcept;
            constexpr Operation(FuncParamCode fcode) noexcept;
            constexpr Operation(ElemParamCode ecode) noexcept;
            constexpr Operation(DataParamCode dcode) noexcept;
            constexpr Operation(AssembeSwitch acode) noexcept;

            operator uint32_t() const noexcept;
            operator PseudoCode() const noexcept;
            operator MacroCode() const noexcept;
            operator BinaryCode() const noexcept;
            operator ImportCode() const noexcept;
            operator GlobalCode() const noexcept;
            operator ImportParamCode() const noexcept;
            operator FuncParamCode() const noexcept;
            operator ElemParamCode() const noexcept;
            operator DataParamCode() const noexcept;
            operator AssembeSwitch() const noexcept;

            bool operator ==(const Operation& op) const noexcept;
            bool operator !=(const Operation& op) const noexcept;
            template <typename T> bool operator ==(T code) const noexcept;
            template <typename T> bool operator !=(T code) const noexcept;

            Operation& operator =(const Operation&) noexcept = default;

            size_t count() const noexcept;
            DataType param(int n) const noexcept;
            DataType result() const noexcept;
            int stack() const noexcept;
        };

        /////////////
        // Operand //
        /////////////

        struct Operand
        {
            uint32_t value;

            Operand() noexcept = default;
            constexpr Operand(const Operand&) noexcept = default;
            constexpr Operand(uint32_t value) noexcept;
            constexpr Operand(int n, int p1, int p2, int p3) noexcept;

            operator uint32_t() const noexcept;
            Operand& operator =(const Operand&) noexcept = default;

            size_t count() const noexcept;
            OperandCode operand(int n) const noexcept;
        };

        ///////////////
        // Parameter //
        ///////////////

        struct Parameter
        {
            Operation operation;
            Operand operand;
            uint16_t length;
            uint16_t binary[5];
        };

        //////////
        // Data //
        //////////

        using Data = StdStringMap<Parameter>;
        using DataIter = Data::const_iterator;

        extern const Data table;

    } // Instruction

    ///////////////
    // InstrIter //
    ///////////////

    using InstrIter = Instruction::DataIter;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    namespace Instruction
    {
        ///////////////
        // Operation //
        ///////////////

        inline constexpr Operation::Operation(int n, int p1, int p2, int p3, int res, int stack) noexcept
            : value(n | (p1 << 4) | (p2 << 8) | (p3 << 12) | (res << 16) | (stack << 20)) {}

        inline constexpr Operation::Operation(uint32_t data) noexcept
            : value(data) {}

        inline constexpr Operation::Operation(PseudoCode pcode) noexcept
            : value(pcode << PSEUDO_CODE_BIT) {}

        inline constexpr Operation::Operation(MacroCode mcode) noexcept
            : value(mcode | (INSTR_MACRO << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(BinaryCode bcode) noexcept
            : value(bcode | (PSEUDO_BINARY << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(ImportCode icode) noexcept
            : value(icode | (PSEUDO_IMPORT << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(GlobalCode gcode) noexcept
            : value(gcode | (PSEUDO_GLOBAL << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(ImportParamCode icode) noexcept
            : value(icode | (PSEUDO_IMPORT_PARAM << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(FuncParamCode fcode) noexcept
            : value(fcode | (PSEUDO_FUNC_PARAM << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(ElemParamCode ecode) noexcept
            : value(ecode | (PSEUDO_ELEM_PARAM << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(DataParamCode dcode) noexcept
            : value(dcode | (PSEUDO_DATA_PARAM << PSEUDO_CODE_BIT)) {}

        inline constexpr Operation::Operation(AssembeSwitch acode) noexcept
            : value(acode | (PSEUDO_ASSMBLE_SWITCH << PSEUDO_CODE_BIT)) {}

        /**/

        inline Operation::operator uint32_t() const noexcept
        {
            return value;
        }

        inline Operation::operator PseudoCode() const noexcept
        {
            return PseudoCode(value >> PSEUDO_CODE_BIT);
        }

        inline Operation::operator MacroCode() const noexcept
        {
            return MacroCode(value >> PSEUDO_CODE_BIT);
        }

        inline Operation::operator BinaryCode() const noexcept
        {
            return BinaryCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator ImportCode() const noexcept
        {
            return ImportCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator GlobalCode() const noexcept
        {
            return GlobalCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator FuncParamCode() const noexcept
        {
            return FuncParamCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator ImportParamCode() const noexcept
        {
            return ImportParamCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator ElemParamCode() const noexcept
        {
            return ElemParamCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator DataParamCode() const noexcept
        {
            return DataParamCode(value & PSEUDO_CODE_MASK);
        }

        inline Operation::operator AssembeSwitch() const noexcept
        {
            return AssembeSwitch(value & PSEUDO_CODE_MASK);
        }

        /**/

        inline bool Operation::operator ==(const Operation& op) const noexcept
        {
            return value == op.value;
        }

        inline bool Operation::operator !=(const Operation& op) const noexcept
        {
            return value != op.value;
        }

        template <typename T>
        inline bool Operation::operator ==(T code) const noexcept
        {
            return T(*this) == code;
        }

        template <typename T>
        inline bool Operation::operator !=(T code) const noexcept
        {
            return T(*this) != code;
        }

        /**/

        inline size_t Operation::count() const noexcept
        {
            return size_t(value & 15);
        }

        inline DataType Operation::param(int n) const noexcept
        {
            assert(0 <= n && n < int(count()));
            return DataType((value >> (n * 4 + 4)) & 15);
        }

        inline DataType Operation::result() const noexcept
        {
            return DataType((value >> 16) & 15);
        }

        inline int Operation::stack() const noexcept
        {
            return DataType((value >> 20) & 15);
        }

        /////////////
        // Operand //
        /////////////

        inline constexpr Operand::Operand(uint32_t data) noexcept
            : value(data) {}

        inline constexpr Operand::Operand(int n, int p1, int p2, int p3) noexcept
            : value(n | (p1 << 4) | (p2 << 10) | (p3 << 16)) {}

        inline Operand::operator uint32_t() const noexcept
        {
            return value;
        }

        inline size_t Operand::count() const noexcept
        {
            return size_t(value & 15);
        }

        inline OperandCode Operand::operand(int n) const noexcept
        {
            assert(0 <= n && n < int(count()));
            return OperandCode((value >> ((n * 6) + 4)) & 63);
        }

    } // Instruction

} // wasmgen
