/*
 * endian.hpp
 */
#pragma once
#include "byteswap.hpp"

#if !WASMGEN_COMPILER_ENDIAN
namespace std
{
    enum class endian
    {
        big    = __ORDER_BIG_ENDIAN__,
        little = __ORDER_LITTLE_ENDIAN__,
        native = __BYTE_ORDER__,
    };

} // std
#endif /* std::endian */

namespace wasmgen
{
    namespace Endian
    {
        //////////////////
        // SwapTypeInfo //
        //////////////////

        template <typename T>
        struct SwapTypeInfo
        {
            enum
            {
                is_floating_point = std::is_floating_point<T>::value,
            };

            template <int N> struct SizeType { using Type = T; };
            template <> struct SizeType<1> { using Type = std::uint8_t; };
            template <> struct SizeType<2> { using Type = std::uint16_t; };
            template <> struct SizeType<4> { using Type = std::uint32_t; };
            template <> struct SizeType<8> { using Type = std::uint64_t; };

            template <bool Float> struct ValueType { using Type = T; };
            template <> struct ValueType<true> { using Type = typename SizeType<sizeof(T)>::Type; };

            using DataType = typename ValueType<is_floating_point>::Type;
        };

        //////////////////////
        // NativeToSwapCast //
        //////////////////////

        template <typename T>
        struct NativeToSwapCast
        {
            using NativeType = T;
            using SwapType = typename SwapTypeInfo<T>::DataType;

            NativeType data;

            inline constexpr NativeToSwapCast(const NativeType& native) noexcept
                : data(native) {}

            inline constexpr operator SwapType() const noexcept
                {
#if !WASMGEN_COMPILER_CXX20
                    return std::byteswap(*(const SwapType*)&data);
#else
                    return std::byteswap(std::bit_cast<SwapType>(data));
#endif
                }
        };

        //////////////////////
        // SwapToNativeCast //
        //////////////////////

        template <typename T>
        struct SwapToNativeCast
        {
            using NativeType = T;
            using SwapType = typename SwapTypeInfo<T>::DataType;

            SwapType data;

            inline constexpr SwapToNativeCast(const SwapType& swap) noexcept
                : data(std::byteswap(swap)) {}

            inline constexpr operator NativeType() const noexcept
                {
#if !WASMGEN_COMPILER_CXX20
                    return *(const NativeType*)&data;
#else
                    return std::bit_cast<NativeType>(data);
#endif
                }
        };

        //////////////////
        // ByteSwapData //
        //////////////////

        template <typename T>
        struct ByteSwapData
        {
            using self = ByteSwapData<T>;
            using value_type = T;

            using SwapInfo = SwapTypeInfo<T>;
            using NativeCast = SwapToNativeCast<T>;
            using SwapCast = NativeToSwapCast<T>;

            typename SwapInfo::DataType data;

            constexpr ByteSwapData() noexcept = default;
            inline constexpr ByteSwapData(const value_type& n) noexcept : data(SwapCast(n)) {}
            constexpr ByteSwapData(const self& s) noexcept = default;

            inline constexpr operator value_type() const noexcept { return get(); }
            inline constexpr bool operator !() const noexcept { return !get(); }
            inline constexpr bool operator ==(const self& s) const noexcept { return data == s.data; }
            inline constexpr bool operator !=(const self& s) const noexcept { return data != s.data; }

            template <typename U> inline constexpr bool operator < (U n) const noexcept { return get() <  n; }
            template <typename U> inline constexpr bool operator <=(U n) const noexcept { return get() <= n; }
            template <typename U> inline constexpr bool operator ==(U n) const noexcept { return get() == n; }
            template <typename U> inline constexpr bool operator !=(U n) const noexcept { return get() != n; }
            template <typename U> inline constexpr bool operator >=(U n) const noexcept { return get() >= n; }
            template <typename U> inline constexpr bool operator > (U n) const noexcept { return get() >  n; }

            inline constexpr value_type operator +() const noexcept { return +get(); }
            inline constexpr value_type operator -() const noexcept { return -get(); }
            inline constexpr value_type operator ~() const noexcept { return ~get(); }

            template <typename U> inline constexpr auto operator +(U n) const noexcept { return get() + n; }
            template <typename U> inline constexpr auto operator -(U n) const noexcept { return get() - n; }
            template <typename U> inline constexpr auto operator *(U n) const noexcept { return get() * n; }
            template <typename U> inline constexpr auto operator /(U n) const noexcept { return get() / n; }
            template <typename U> inline constexpr auto operator %(U n) const noexcept { return get() % n; }

            template <typename U> inline constexpr auto operator &(U n) const noexcept { return get() & n; }
            template <typename U> inline constexpr auto operator |(U n) const noexcept { return get() | n; }
            template <typename U> inline constexpr auto operator ^(U n) const noexcept { return get() ^ n; }

            inline constexpr value_type operator <<(int n) const noexcept { return get() << n; }
            inline constexpr value_type operator >>(int n) const noexcept { return get() >> n; }

            inline constexpr self& operator =(const value_type& n) noexcept { return set(n); }
            inline constexpr self& operator =(const self& s) noexcept = default;

            template <typename U> inline constexpr self& operator +=(U n) noexcept { return set(get() + n); }
            template <typename U> inline constexpr self& operator -=(U n) noexcept { return set(get() - n); }
            template <typename U> inline constexpr self& operator *=(U n) noexcept { return set(get() * n); }
            template <typename U> inline constexpr self& operator /=(U n) noexcept { return set(get() / n); }
            template <typename U> inline constexpr self& operator %=(U n) noexcept { return set(get() % n); }

            inline constexpr self& operator &=(const value_type& n) noexcept { return set(get() & n); }
            inline constexpr self& operator |=(const value_type& n) noexcept { return set(get() | n); }
            inline constexpr self& operator ^=(const value_type& n) noexcept { return set(get() ^ n); }

            inline constexpr self& operator <<=(int n) noexcept { return set(get() << n); }
            inline constexpr self& operator >>=(int n) noexcept { return set(get() >> n); }

            inline constexpr T get() const noexcept { return NativeCast(data); }
            inline constexpr self& set(const T& n) noexcept { data = SwapCast(n); return *this; }
        };

        /////////////
        // Wrapper //
        /////////////

        struct NativeWrapper { template <typename T> using Type = T; };
        struct SwapWrapper { template <typename T> using Type = ByteSwapData<T>; };

        ////////////////
        // DataAccess //
        ////////////////

        template <typename Wrapper>
        struct DataAccess
        {
            static inline const void *byte_offset(const void* p, off_t o) noexcept
                { return (const uint8_t*)p + o; }

            template <typename T>
            static inline T read(const void* p, off_t o = 0) noexcept
                {
                    using source = typename Wrapper::template Type<T>;

                    return *(const source*)byte_offset(p, o);
                }

            template <typename T>
            static inline void* write(const T& v, void* p, off_t o = 0) noexcept
                {
                    using target = typename Wrapper::template Type<T>;

                    (*(target *)byte_offset(p, o)) = v;
                    return p;
                }
        };

        ////////////////
        // NativeType //
        ////////////////

        struct NativeType : DataAccess<NativeWrapper>
        {
            using Int8 = int8_t;
            using Int16 = int16_t;
            using Int32 = int32_t;
            using Int64 = int64_t;

            using UInt8 = uint8_t;
            using UInt16 = uint16_t;
            using UInt32 = uint32_t;
            using UInt64 = uint64_t;

            using Float32 = float;
            using Float64 = double;
        };

        //////////////
        // SwapType //
        //////////////

        struct SwapType : DataAccess<SwapWrapper>
        {
            using Int8  = ByteSwapData<int8_t>;
            using Int16 = ByteSwapData<int16_t>;
            using Int32 = ByteSwapData<int32_t>;
            using Int64 = ByteSwapData<int64_t>;

            using UInt8  = ByteSwapData<uint8_t>;
            using UInt16 = ByteSwapData<uint16_t>;
            using UInt32 = ByteSwapData<uint32_t>;
            using UInt64 = ByteSwapData<uint64_t>;

            using Float32 = ByteSwapData<float>;
            using Float64 = ByteSwapData<double>;
        };

        ////////////////
        // EndianType //
        ////////////////

        template <bool NATIVE>
        using EndianType = typename
            std::conditional<NATIVE, NativeType, SwapType>::type;

        //////////////////
        // Big / Little //
        //////////////////

        using Big = EndianType<std::endian::native == std::endian::big>;
        using Little = EndianType<std::endian::native == std::endian::little>;

    } // Endian

} // wasmgen

namespace wasmgen
{
    ///////////////
    // BigEndian //
    ///////////////

    using BigEndian = Endian::Big;

    using int8be_t  = BigEndian::Int8;
    using int16be_t = BigEndian::Int16;
    using int32be_t = BigEndian::Int32;
    using int64be_t = BigEndian::Int64;

    using uint8be_t  = BigEndian::UInt8 ;
    using uint16be_t = BigEndian::UInt16;
    using uint32be_t = BigEndian::UInt32;
    using uint64be_t = BigEndian::UInt64;

    using float32be_t = BigEndian::Float32;
    using float64be_t = BigEndian::Float64;

    //////////////////
    // LittleEndian //
    //////////////////

    using LittleEndian = Endian::Little;

    using int8le_t  = LittleEndian::Int8;
    using int16le_t = LittleEndian::Int16;
    using int32le_t = LittleEndian::Int32;
    using int64le_t = LittleEndian::Int64;

    using uint8le_t  = LittleEndian::UInt8 ;
    using uint16le_t = LittleEndian::UInt16;
    using uint32le_t = LittleEndian::UInt32;
    using uint64le_t = LittleEndian::UInt64;

    using float32le_t = LittleEndian::Float32;
    using float64le_t = LittleEndian::Float64;

} // wasmgen
