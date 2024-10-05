/*
 * data.hpp
 */
#pragma once
#include "array.hpp"
#include "string.hpp"

namespace wasmgen
{
    /////////////
    // VALTYPE //
    /////////////

    enum class ValType : int8_t
    {
        NONE = 0,
        I32 = -1,
        I64 = -2,
        F32 = -3,
        F64 = -4,
        V128 = -5,
        FUNCREF = -16,
        EXTERNREF = -17,
        FUNCTYPE = -32,
        EMPTY = -64,
    };

    using NumType = ValType;
    using VecType = ValType;
    using RefType = ValType;

    using ResultType = StdVector<ValType>;

    /**/

    using ValTypeDict = StdStringMap<ValType>;
    using ValTypeName = StdMap<ValType, StdString>;

    extern const ValTypeDict numtype_dict;
    extern const ValTypeDict vectype_dict;
    extern const ValTypeDict reftype_dict;
    extern const ValTypeDict valtype_dict;
    extern const ValTypeDict functype_dict;

    extern const ValTypeName valtype_name;

    //////////////
    // FuncType //
    //////////////

    struct FuncType
    {
        ResultType arg;
        ResultType res;

        bool operator <(const FuncType& rhs) const noexcept;
        bool operator ==(const FuncType& rhs) const noexcept;
        bool operator !=(const FuncType& rhs) const noexcept;
    };

    //////////////////
    // FuncTypeList //
    //////////////////

    using FuncTypeList = StdVector<FuncType>;

    /////////////////
    // FuncTypeMap //
    /////////////////

    class FuncTypeMap : public StdMap<FuncType, int>
    {
    private:
        using super = StdMap<FuncType, int>;

    public:
        using super::super;

        bool has(const key_type& key) const;
    };

    ///////////////
    // DataIndex //
    ///////////////

    using DataIndex = uint32_t;

    using TypeIdx = DataIndex;
    using FuncIdx = DataIndex;
    using TableIdx = DataIndex;
    using MemIdx = DataIndex;
    using GlobalIdx = DataIndex;
    using ElemIdx = DataIndex;
    using DataIdx = DataIndex;
    using LocalIdx = DataIndex;
    using LabelIdx = DataIndex;

    /////////////////
    // DataIndices //
    /////////////////

    using DataIndexList = StdVector<DataIndex>;

    using TypeIdxList = DataIndexList;
    using FuncIdxList = DataIndexList;
    using TableIdxList = DataIndexList;
    using MemIdxList = DataIndexList;
    using GlobalIdxList = DataIndexList;
    using ElemIdxList = DataIndexList;
    using DataIdxList = DataIndexList;
    using LocalIdxList = DataIndexList;
    using LabelIdxList = DataIndexList;

    ////////////
    // Limits //
    ////////////

    struct Limits
    {
        int mode;
        uint32_t min;
        uint32_t max;

        Limits() noexcept = default;
        constexpr Limits(uint32_t min) noexcept;
        constexpr Limits(uint32_t min, uint32_t max) noexcept;

        constexpr void set(uint32_t min) noexcept;
        constexpr void set(uint32_t min, uint32_t max) noexcept;

        StringBlock dump() const;
    };

    using MemType = Limits;

    ///////////////
    // ExprValue //
    ///////////////

    struct ExprValue
    {
        enum Type
        {
            ST_NONE,

            ST_INT,
            ST_FLOAT,

            ST_NAME,
            ST_NULL,
            ST_VALTYPE,
            ST_STRING,
        };

        Type type;
        int64_t ivalue;
        double fvalue;
        StringPtr string;

        ExprValue() noexcept;
        ExprValue(Type type) noexcept;
        ExprValue(ValType vt) noexcept;
        ExprValue(bool i) noexcept;
        ExprValue(int64_t i) noexcept;
        ExprValue(uint64_t i) noexcept : ExprValue(int64_t(i)) {}
        ExprValue(double f) noexcept;
        ExprValue(String* str, Type type = ST_STRING) noexcept;

        bool isvalid() const noexcept;

        bool isint() const noexcept;
        bool isfloat() const noexcept;
        bool isnumber() const noexcept;

        bool isname() const noexcept;
        bool isnull() const noexcept;
        bool isvaltype() const noexcept;
        bool isstring() const noexcept;

        operator bool() const noexcept;
        operator int8_t() const noexcept;
        operator uint8_t() const noexcept;
        operator int16_t() const noexcept;
        operator uint16_t() const noexcept;
        operator int32_t() const noexcept;
        operator uint32_t() const noexcept;
        operator int64_t() const noexcept;
        operator uint64_t() const noexcept;
        operator float() const noexcept;
        operator double() const noexcept;
        operator const String*() const noexcept;

        StdString str() const;
        StdString dump() const;
    };

    using ExprValueList = StdVector<ExprValue>;
    using ExprValueListList = StdVector<ExprValueList>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // Limits //
    ////////////

    inline constexpr Limits::Limits(uint32_t min) noexcept
        : mode(0), min(min), max(0) {}

    inline constexpr Limits::Limits(uint32_t min, uint32_t max) noexcept
        : mode(1), min(min), max(max) {}

    inline constexpr void Limits::set(uint32_t mini) noexcept
    {
        mode = 0;
        min = mini;
        max = 0;
    }

    inline constexpr void Limits::set(uint32_t mini, uint32_t maxi) noexcept
    {
        mode = 1;
        min = mini;
        max = maxi;
    }

    /**/

    inline StringBlock Limits::dump() const
    {
        return StringBlock {"min=", min, (mode ? StringBlock {", max=", max} : StringBlock {})};
    }

    //////////////
    // FuncType //
    //////////////

    inline bool FuncType::operator <(const FuncType& rhs) const noexcept
    {
        return (arg != rhs.arg) ? (arg < rhs.arg) : (res < rhs.res);
    }

    inline bool FuncType::operator ==(const FuncType& rhs) const noexcept
    {
        return arg == rhs.arg && res == rhs.res;
    }

    inline bool FuncType::operator !=(const FuncType& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    /////////////////
    // FuncTypeMap //
    /////////////////

    inline bool FuncTypeMap::has(const key_type& key) const
    {
#if !WASMGEN_COMPILER_CXX20
        return find(key) != end();
#else /* C++20 */
        return contains(key);
#endif
    }

    /////////////////
    // ExprValue //
    /////////////////

    inline ExprValue::ExprValue() noexcept
        : type(ST_NONE)
        , ivalue(0)
        , fvalue(0)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(Type type) noexcept
        : type(type)
        , ivalue(0)
        , fvalue(0)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(ValType vt) noexcept
        : type(ST_INT)
        , ivalue(int(vt))
        , fvalue(0)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(bool i) noexcept
        : type(ST_INT)
        , ivalue(i)
        , fvalue(0)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(int64_t i) noexcept
        : type(ST_INT)
        , ivalue(i)
        , fvalue(0)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(double f) noexcept
        : type(ST_FLOAT)
        , ivalue(0)
        , fvalue(f)
    {
        /*NOOP*/
    }

    inline ExprValue::ExprValue(String* s, Type t) noexcept
        : type(t)
        , ivalue(0)
        , fvalue(0)
        , string(s)
    {
        /*NOOP*/
    }

    /**/

    inline bool ExprValue::isvalid() const noexcept
    {
        return type != ST_NONE;
    }

    inline bool ExprValue::isint() const noexcept
    {
        return type == ST_INT;
    }

    inline bool ExprValue::isfloat() const noexcept
    {
        return type == ST_FLOAT;
    }

    inline bool ExprValue::isnumber() const noexcept
    {
        return isint() || isfloat();
    }

    inline bool ExprValue::isname() const noexcept
    {
        return type == ST_NAME;
    }

    inline bool ExprValue::isnull() const noexcept
    {
        return type == ST_NULL;
    }

    inline bool ExprValue::isvaltype() const noexcept
    {
        return type == ST_VALTYPE;
    }

    inline bool ExprValue::isstring() const noexcept
    {
        return type == ST_STRING;
    }

    /**/

    inline ExprValue::operator bool() const noexcept
    {
        if (isint())
            return !!ivalue;
        if (isfloat())
            return !!fvalue;
        if (isstring())
            return !!string;
        return false;
    }

    inline ExprValue::operator int8_t() const noexcept
    {
        return int8_t(int64_t(*this));
    }

    inline ExprValue::operator uint8_t() const noexcept
    {
        return uint8_t(uint64_t(*this));
    }

    inline ExprValue::operator int16_t() const noexcept
    {
        return int16_t(int64_t(*this));
    }

    inline ExprValue::operator uint16_t() const noexcept
    {
        return uint16_t(uint64_t(*this));
    }

    inline ExprValue::operator int32_t() const noexcept
    {
        return int32_t(int64_t(*this));
    }

    inline ExprValue::operator uint32_t() const noexcept
    {
        return uint32_t(uint64_t(*this));
    }

    inline ExprValue::operator int64_t() const noexcept
    {
        if (isint())
            return int64_t(ivalue);
        if (isfloat())
            return int64_t(fvalue);
        assert(false);
        return 0;
    }

    inline ExprValue::operator uint64_t() const noexcept
    {
        if (isint())
            return uint64_t(ivalue);
        if (isfloat())
            return uint64_t(fvalue);
        assert(false);
        return 0;
    }

    inline ExprValue::operator float() const noexcept
    {
        if (isint())
            return float(ivalue);
        if (isfloat())
            return float(fvalue);
        assert(false);
        return 0;
    }

    inline ExprValue::operator double() const noexcept
    {
        if (isint())
            return double(ivalue);
        if (isfloat())
            return double(fvalue);
        assert(false);
        return 0;
    }

    inline ExprValue::operator const String*() const noexcept
    {
        if (isstring())
            return &string;
        assert(false);
        return nullptr;
    }

} // wasmgen
