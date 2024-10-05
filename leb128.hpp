/*
 * leb128.hpp
 */
#pragma once
#include "bitscan.hpp"

namespace wasmgen
{
    ////////////
    // LEB128 //
    ////////////

    struct LEB128
    {
        uint8_t size;
        uint8_t data[11];

        LEB128() noexcept = default;
        template <typename T> constexpr LEB128(T x) noexcept;
        constexpr LEB128(const LEB128& s) noexcept = default;

        template <typename T> constexpr LEB128& encode(T x) noexcept;
        template <typename T> constexpr LEB128& encode(T x, int bits) noexcept;
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // LEB128 //
    ////////////

    template <typename T>
    inline constexpr LEB128::LEB128(T x) noexcept
    {
        encode(x);
    }

    template <typename T>
    inline constexpr LEB128& LEB128::encode(T x) noexcept
    {
        return encode(x, BitScanReverse(x < 0 ? ~x : x) + 1
                      + (std::is_signed<T>::value ? 1 : 0));
    }

    template <typename T>
    inline constexpr LEB128& LEB128::encode(T x, int bits) noexcept
    {
        using U = std::make_unsigned_t<T>;

        U y = U(x);

        size = 0; std::memset(data, 0, sizeof(data));
        for (size = 0; bits > 7; bits -= 7, y >>= 7)
            data[size++] = uint8_t(y & 0x7f) | 0x80;
        data[size++] = uint8_t(y & 0x7f);
        return *this;
    }

} // wasmgen
