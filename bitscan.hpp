/*
 * bitscan.hpp
 */
#pragma once
#include "compiler.hpp"

#if !WASMGEN_TARGET_WINDOWS
#define WASMGEN_BITSCANREVERSE_CONSTEXPR  constexpr
#else
#define WASMGEN_BITSCANREVERSE_CONSTEXPR
#endif

namespace wasmgen
{
    ////////////////////
    // BitScanReverse //
    ////////////////////

    WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse32(uint32_t x) noexcept;
    WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse64(uint64_t x) noexcept;
    template <typename T> WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse(T x) noexcept;
    template <typename T> WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverseStep(T x, int r, int b) noexcept;

    /**/

    template <typename T>
    inline WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverseStep(T x, int r, int b) noexcept
    {
        return (!b ? r + int(x) : (x >> b)
                ? BitScanReverseStep(x >> b, r + b, b >> 1)
                : BitScanReverseStep(x >> 0, r + 0, b >> 1));
    }

    inline WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse32(uint32_t x) noexcept
    {
#if WASMGEN_TARGET_WINDOWS
        unsigned long r;
        return _BitScanReverse(&r, x) ? int(r) : -1;
#elif WASMGEN_COMPILER_BUILTIN_CLZ
        return !x ? -1 : 31 - __builtin_clz(x);
#elif WASMGEN_COMPILER_COUNTL_ZERO
        return 31 - std::countl_zero(x);
#else
        return BitScanReverseStep(x, -1, 16);
#endif
    }

    inline WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse64(uint64_t x) noexcept
    {
#if WASMGEN_TARGET_WINDOWS
        unsigned long r;
        return _BitScanReverse64(&r, x) ? int(r) : -1;
#elif WASMGEN_COMPILER_BUILTIN_CLZLL
        return !x ? -1 : 63 - __builtin_clzll(x);
#elif WASMGEN_COMPILER_COUNTL_ZERO
        return 63 - std::countl_zero(x);
#else
        return BitScanReverseStep(x, -1, 32);
#endif
    }

    template <typename T>
    inline WASMGEN_BITSCANREVERSE_CONSTEXPR int BitScanReverse(T x) noexcept
    {
        return (sizeof(T) <= 4)
            ? BitScanReverse32(uint32_t(x))
            : BitScanReverse64(uint64_t(x));
    }

} // wasmgen

#undef WASMGEN_BITSCANREVERSE_CONSTEXPR
