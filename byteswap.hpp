/*
 * byteswap.hpp
 */
#pragma once
#include "compiler.hpp"

#ifndef WASMGEN_COMPILER_BSWAP_BUILTIN
#define WASMGEN_COMPILER_BSWAP_BUILTIN  1
#endif

#if !WASMGEN_COMPILER_BYTESWAP
namespace std
{
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4067 4293 4333 26452)
#endif /* MSVC */

    template <typename T>
    inline constexpr T byteswap(T x) noexcept
    {
        return (sizeof(T) == 8 ?
#if WASMGEN_COMPILER_BSWAP_BUILTIN && defined(_MSC_VER)
                T(_byteswap_uint64((unsigned __int64)x))
#elif WASMGEN_COMPILER_BSWAP_BUILTIN && __has_builtin(__builtin_bswap64)
                T(__builtin_bswap64(x))
#else /* C */
                (((x >> 000) & 0xff) << 070) |
                (((x >> 010) & 0xff) << 060) |
                (((x >> 020) & 0xff) << 050) |
                (((x >> 030) & 0xff) << 040) |
                (((x >> 040) & 0xff) << 030) |
                (((x >> 050) & 0xff) << 020) |
                (((x >> 060) & 0xff) << 010) |
                (((x >> 070) & 0xff) << 000)
#endif /* C */
                : sizeof(T) == 4 ?
#if WASMGEN_COMPILER_BSWAP_BUILTIN && defined(_MSC_VER)
                T(_byteswap_ulong((unsigned long)x))
#elif WASMGEN_COMPILER_BSWAP_BUILTIN && __has_builtin(__builtin_bswap32)
                T(__builtin_bswap32(x))
#else /* C */
                (((x >> 000) & 0xff) << 030) |
                (((x >> 010) & 0xff) << 020) |
                (((x >> 020) & 0xff) << 010) |
                (((x >> 030) & 0xff) << 000)
#endif /* C */
                : sizeof(T) == 2 ?
#if WASMGEN_COMPILER_BSWAP_BUILTIN && defined(_MSC_VER)
                T(_byteswap_ushort((unsigned short)x))
#elif WASMGEN_COMPILER_BSWAP_BUILTIN && __has_builtin(__builtin_bswap16)
                T(__builtin_bswap16(x))
#else  /* C */
                (((x >> 000) & 0xff) << 010) |
                (((x >> 010) & 0xff) << 000)
#endif /* C */
                : x);
    }

#ifdef _MSC_VER
#pragma warning(pop)
#endif /* MSVC */
} // std
#endif /* std::byteswap */
