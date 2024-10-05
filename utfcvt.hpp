/*
 * utfcvt.hpp
 */
#pragma once

/*
 * __has_builtin
 */

#ifndef __has_builtin
#define __has_builtin(f)  0
#endif

/*
 * GCC / Clang
 */

#ifndef utfcvt_compiler_gnu_extension
#  if defined(__GNUC__) || defined(__clang__)
#    define utfcvt_compiler_gnu_extension  1
#  else
#    define utfcvt_compiler_gnu_extension  0
#  endif
#endif
#if !utfcvt_compiler_gnu_extension && !defined(__attribute__)
#define __attribute__(x)  /*NOOP*/
#endif

/*
 * C++XX
 */

#if __cplusplus >= 201103L || _MSVC_LANG >= 201103L
#  define utfcvt_compiler_cxx11  1
#else  /* C++11 */
#  define utfcvt_compiler_cxx11  0
#endif

#if __cplusplus >= 201402L || _MSVC_LANG >= 201402L
#  define utfcvt_compiler_cxx14  1
#else  /* C++11 */
#  define utfcvt_compiler_cxx14  0
#endif

#if __cplusplus >= 201703L || _MSVC_LANG >= 201703L
#  define utfcvt_compiler_cxx17  1
#else  /* C++11 */
#  define utfcvt_compiler_cxx17  0
#endif

#if __cplusplus >= 202002L || _MSVC_LANG >= 202002L
#  define utfcvt_compiler_cxx20  1
#else  /* C++20 */
#  define utfcvt_compiler_cxx20  0
#endif

/*
 *
 */

#include <algorithm>
#include <cstdint>
#include <cwchar>
#include <limits>
#include <string>

#ifdef _MSC_VER
#  include <intrin.h>
#endif

#if utfcvt_compiler_cxx20
#include <bit>
#endif

/*
 * constexpr / noexcept
 */

#ifndef utfcvt_constexpr
#  if !utfcvt_compiler_cxx11
#    define utfcvt_constexpr  __attribute__((const, pure))
#  else
#    define utfcvt_constexpr  constexpr
#  endif
#endif
#if !utfcvt_compiler_cxx11
#  ifndef noexcept
#    define utfcvt_defined_noexcept  1
#    define noexcept
#  endif
#endif

/*
 * char / wchar / char8_t / char16_t / char32_t
 */
#define utfcvt_compiler_cxx_char  1
#define utfcvt_compiler_cxx_wchar  1

#if __cpp_char8_t >= 201811L
#  define utfcvt_compiler_cxx_char8  1
#else
#  define utfcvt_compiler_cxx_char8  0
#endif

#if (utfcvt_compiler_cxx11 ||                   \
     (defined(_MSC_VER) && _MSC_VER >= 1900) || \
     defined(__clang__) ||                      \
     0)
#  define utfcvt_compiler_cxx_char16  1
#  define utfcvt_compiler_cxx_char32  1
#else
#  define utfcvt_compiler_cxx_char16  0
#  define utfcvt_compiler_cxx_char32  0
#endif

#ifndef utfcvt_compiler_char
#  define utfcvt_compiler_char  utfcvt_compiler_cxx_char
#endif
#ifndef utfcvt_compiler_wchar
#  define utfcvt_compiler_wchar  utfcvt_compiler_cxx_wchar
#endif
#ifndef utfcvt_compiler_char8
#  define utfcvt_compiler_char8  utfcvt_compiler_cxx_char8
#endif
#ifndef utfcvt_compiler_char16
#  define utfcvt_compiler_char16  utfcvt_compiler_cxx_char16
#endif
#ifndef utfcvt_compiler_char32
#  define utfcvt_compiler_char32  utfcvt_compiler_cxx_char32
#endif

#define utfcvt_compiler_cxx_string  utfcvt_compiler_cxx_char
#define utfcvt_compiler_cxx_wstring  utfcvt_compiler_cxx_wchar
#define utfcvt_compiler_cxx_u8string  utfcvt_compiler_cxx_char8
#define utfcvt_compiler_cxx_u16string  utfcvt_compiler_cxx_char16
#define utfcvt_compiler_cxx_u32string  utfcvt_compiler_cxx_char32

#ifndef utfcvt_compiler_string
#  define utfcvt_compiler_string  utfcvt_compiler_cxx_string
#endif
#ifndef utfcvt_compiler_wstring
#  define utfcvt_compiler_wstring  utfcvt_compiler_cxx_wstring
#endif
#ifndef utfcvt_compiler_u8string
#  define utfcvt_compiler_u8string  utfcvt_compiler_cxx_u8string
#endif
#ifndef utfcvt_compiler_u16string
#  define utfcvt_compiler_u16string  utfcvt_compiler_cxx_u16string
#endif
#ifndef utfcvt_compiler_u32string
#  define utfcvt_compiler_u32string  utfcvt_compiler_cxx_u32string
#endif

/*
 * bsr
 */
#ifndef utfcvt_compiler_bsr
#  if 0 /* utfcvt_compiler_cxx20 */
#    define utfcvt_compiler_bsr  1
#  else
#    if defined(__GNUC__) && (defined(__x86__) || defined(__x86_64))
#      define utfcvt_compiler_bsr_x86  1
#    endif
#    ifdef _MSC_VER
#      define utfcvt_compiler_bsr_msvc  1
#    endif /* VC++ */
#    if (0 ||                                   \
         defined(utfcvt_compiler_bsr_x86) ||    \
         defined(utfcvt_compiler_bsr_msvc) ||   \
         0)
#      define utfcvt_compiler_bsr  1
#    endif
#  endif
#endif
#ifndef utfcvt_compiler_bsr
#  define utfcvt_compiler_bsr  0
#endif

/*
 * clz
 */
#ifndef utfcvt_compiler_clz
#  if utfcvt_compiler_cxx20
#    define utfcvt_compiler_clz  1
#  else
#    if __has_builtin(__builtin_clz)
#      define utfcvt_compiler_builtin_clz  1
#    endif
#    if (0 ||                                       \
         defined(utfcvt_compiler_builtin_clz) ||    \
         defined(utfcvt_compiler_bsr) ||            \
         0)
#      define utfcvt_compiler_clz  1
#    endif
#  endif
#endif
#ifndef utfcvt_compiler_clz
#  define utfcvt_compiler_clz  0
#endif

/*
 * countl_zero
 */
#ifndef utfcvt_compiler_countl_zero
#  if __cpp_lib_bitops < 201907L
#    define utfcvt_compiler_countl_zero  0
#  else
#    define utfcvt_compiler_countl_zero  1
#  endif
#endif

/*
 * utf8::putcodelen
 */
#ifndef utfcvt_utf8_putcodelen_mode
#  if !(!utfcvt_compiler_bsr || utfcvt_compiler_clz)
#    define utfcvt_utf8_putcodelen_mode  0
#  elif !utfcvt_compiler_cxx11 /* || utfcvt_compiler_cxx2b */
#    define utfcvt_utf8_putcodelen_mode  2
#  else
#    define utfcvt_utf8_putcodelen_mode  1
#  endif
#endif

/*
 * Declaration
 */
namespace utfcvt
{
    typedef std::uint32_t utf32_t;

    struct utfcvt_getcres;
    struct utfcvt_result;

#if !utfcvt_compiler_cxx17
    typedef utfcvt_getcres (*utfcvt_nextcode)(const void*& s, const void* e);
#else  /* C++17 */
    typedef utfcvt_getcres (*utfcvt_nextcode)(const void*& s, const void* e) noexcept;
#endif

    /*
     *
     */

    utfcvt_constexpr std::uint8_t replace_u8c() noexcept;
    utfcvt_constexpr std::uint16_t replace_u16c() noexcept;
    utfcvt_constexpr std::uint32_t replace_u32c() noexcept;

    template <typename sT> size_t utflen(sT* s) noexcept;

    template <typename dT, typename iT> dT convert(iT s, iT e);
    template <typename dT, typename sT> dT convert(sT* s, size_t n);
    template <typename dT, typename sT> dT convert(sT& s);
    template <typename dT, typename sT> dT convert(sT* s);

    template <typename dT> dT convert(utfcvt_nextcode f, const void* s, const void* e);
    template <typename dT> dT convert(utfcvt_nextcode f, const void* s, size_t n);

#if utfcvt_compiler_string
    template <typename sT> std::string to_string(sT s);
    template <typename iT> std::string to_string(iT s, iT e);
    template <typename sT> std::string to_string(sT* s, size_t n);
#endif

#if utfcvt_compiler_wstring
    template <typename sT> std::wstring to_wstring(sT s);
    template <typename iT> std::wstring to_wstring(iT s, iT e);
    template <typename sT> std::wstring to_wstring(sT* s, size_t n);
#endif

#if utfcvt_compiler_u8string
    template <typename sT> std::u8string to_u8string(sT s);
    template <typename iT> std::u8string to_u8string(iT s, iT e);
    template <typename sT> std::u8string to_u8string(sT* s, size_t n);
#endif

#if utfcvt_compiler_u16string
    template <typename sT> std::u16string to_u16string(sT s);
    template <typename iT> std::u16string to_u16string(iT s, iT e);
    template <typename sT> std::u16string to_u16string(sT* s, size_t n);
#endif

#if utfcvt_compiler_u32string
    template <typename sT> std::u32string to_u32string(sT s);
    template <typename iT> std::u32string to_u32string(iT s, iT e);
    template <typename sT> std::u32string to_u32string(sT* s, size_t n);
#endif

    /*
     *
     */

    struct utfcvt_getcres
    {
        bool success;
        std::uint8_t size;
        utf32_t code;

        utfcvt_getcres() noexcept;
        utfcvt_getcres(size_t s, utf32_t c, bool b = true) noexcept;
    };

    struct utfcvt_result
    {
        bool success;
        size_t read;
        size_t write;

        utfcvt_result() noexcept;
        utfcvt_result(size_t r, size_t w, bool b = true) noexcept;
    };

    /*
     *
     */

    namespace utfcvtimpl
    {
        template <typename T> utfcvt_constexpr int bsr32(T t) noexcept;

        template <typename T> utfcvt_constexpr int clzN(T x) noexcept;
        template <typename T> utfcvt_constexpr int clz32(T t) noexcept;

        /**/

        template <typename T>
        struct binary_type
        {
            typedef T value_type;
            enum { size = sizeof(T) };
        };

        template <typename T> struct data_type;

#if utfcvt_compiler_cxx_char
        template <> struct data_type<char> : binary_type<char> {};
#endif
#if utfcvt_compiler_cxx_wchar
        template <> struct data_type<wchar_t> : binary_type<wchar_t> {};
#endif
#if utfcvt_compiler_cxx_char8
        template <> struct data_type<char8_t> : binary_type<char8_t> {};
#endif
#if utfcvt_compiler_cxx_char16
        template <> struct data_type<char16_t> : binary_type<char16_t> {};
#endif
#if utfcvt_compiler_cxx_char32
        template <> struct data_type<char32_t> : binary_type<char32_t> {};
#endif

        template <> struct data_type<signed char> : binary_type<signed char> {};
        template <> struct data_type<signed short> : binary_type<signed short> {};
        template <> struct data_type<signed int> : binary_type<signed int> {};
        template <> struct data_type<signed long> : binary_type<signed long> {};
        template <> struct data_type<signed long long> : binary_type<signed long long> {};

        template <> struct data_type<unsigned char> : binary_type<unsigned char> {};
        template <> struct data_type<unsigned short> : binary_type<unsigned short> {};
        template <> struct data_type<unsigned int> : binary_type<unsigned int> {};
        template <> struct data_type<unsigned long> : binary_type<unsigned long> {};
        template <> struct data_type<unsigned long long> : binary_type<unsigned long long> {};

        template <typename T> struct data_type<T*> : data_type<T> {};
        template <typename T> struct data_type<T[]> : data_type<T> {};
        template <typename T> struct data_type<T&> : data_type<T> {};
        template <typename T> struct data_type<const T> : data_type<T> {};
        template <typename T> struct data_type<volatile T> : data_type<T> {};

        template <typename T>
        struct data_type
        {
            typedef typename data_type<typename T::value_type>::value_type value_type;
            enum { size = sizeof(value_type) };
        };

        template <typename T>
        struct code_type
        {
            typedef data_type<T> data_type;

            enum { size = data_type::size };

            template <int N>
            struct size_type;

            template <>
            struct size_type<1>
            {
                typedef std::uint8_t char_t;
                typedef std::uint32_t comp_t;
            };

            template <>
            struct size_type<2>
            {
                typedef std::uint16_t char_t;
                typedef std::uint32_t comp_t;
            };

            template <>
            struct size_type<4>
            {
                typedef std::uint32_t char_t;
                typedef std::uint32_t comp_t;
            };

            template <>
            struct size_type<8>
            {
                typedef std::uint64_t char_t;
                typedef std::uint64_t comp_t;
            };

            typedef size_type<size> type_data;
            typedef typename type_data::char_t char_t;
            typedef typename type_data::comp_t comp_t;
        };

        /**/

        class utf_common
        {
        public:
            template <typename sT>
            static size_t len(sT* s) noexcept;

            template <typename sT>
            static size_t length(sT* s) noexcept;

        protected:
            template <typename dU, typename sU, typename dT, typename iT>
            static utfcvt_result append(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result copy(dT& d, iT s, iT e);

            template <typename dU, typename dT, typename iT>
            static utfcvt_result degrade(dT& d, iT s, iT e);

            template <typename dU, typename sU, typename dT, typename iT>
            static utfcvt_result convert(dT& d, iT s, iT e);

            template <typename dU, typename sU, typename iT>
            static utfcvt_result count(iT s, iT e) noexcept;

            template <typename dU, typename sU, typename iT>
            static utfcvt_result cvtlen(iT s, iT e) noexcept;
        };

        class utf8 : public utf_common
        {
        public:
            enum { code_size = 1 };

            static utfcvt_constexpr std::uint8_t replacement_character() noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen(cT c) noexcept;

            template <typename iT>
            static utfcvt_getcres getcode(iT s, iT e) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen(cT c) noexcept;

            template <typename dT, typename cT>
            static size_t putcode(dT& d, cT c);

            template <typename iT>
            static utfcvt_result to_utf8len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf16len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf32len(iT s, iT e) noexcept;

            template <typename dT, typename iT>
            static utfcvt_result to_utf8(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf16(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf32(dT& d, iT s, iT e);

        protected:
            template <typename cT>
            static utfcvt_constexpr size_t getcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen64(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen64(cT c) noexcept;
        };

        class utf16 : public utf_common
        {
        public:
            enum { code_size = 2 };

            static utfcvt_constexpr std::uint16_t replacement_character() noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen(cT c) noexcept;

            template <typename iT>
            static utfcvt_getcres getcode(iT s, iT e) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen(cT c) noexcept;

            template <typename dT, typename cT>
            static size_t putcode(dT& d, cT c);

            template <typename iT>
            static utfcvt_result to_utf8len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf16len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf32len(iT s, iT e) noexcept;

            template <typename dT, typename iT>
            static utfcvt_result to_utf8(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf16(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf32(dT& d, iT s, iT e);

        protected:
            template <typename cT>
            static utfcvt_constexpr size_t getcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen64(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen64(cT c) noexcept;
        };

        class utf32 : public utf_common
        {
        public:
            enum { code_size = 4 };

            static utfcvt_constexpr std::uint32_t replacement_character() noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen(cT c) noexcept;

            template <typename iT>
            static utfcvt_getcres getcode(iT s, iT e) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen(cT c) noexcept;

            template <typename dT, typename cT>
            static size_t putcode(dT& d, cT c);

            template <typename iT>
            static utfcvt_result to_utf8len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf16len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf32len(iT s, iT e) noexcept;

            template <typename dT, typename iT>
            static utfcvt_result to_utf8(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf16(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf32(dT& d, iT s, iT e);

        protected:
            template <typename cT>
            static utfcvt_constexpr size_t getcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen64(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen32(cT c) noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t putcodelen64(cT c) noexcept;
        };

        class utf : public utf_common
        {
        public:
            template <typename cT>
            static utfcvt_constexpr cT replacement_character() noexcept;

            template <typename cT>
            static utfcvt_constexpr size_t getcodelen(cT c) noexcept;

            template <typename iT>
            static utfcvt_getcres getcode(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_getcres nextcode(iT& s, iT e) noexcept;

            template <typename cT>
            inline static utfcvt_constexpr size_t putcodelen(cT c) noexcept;

            template <typename dT, typename cT>
            static size_t putcode(dT& d, cT c);

            template <typename iT>
            static utfcvt_result to_utf8len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf16len(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result to_utf32len(iT s, iT e) noexcept;

            template <typename dT, typename iT>
            static utfcvt_result to_utf8(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf16(dT& d, iT s, iT e);

            template <typename dT, typename iT>
            static utfcvt_result to_utf32(dT& d, iT s, iT e);

            /* cvtlen */

            template <typename dT, typename iT>
            static utfcvt_result cvtlen(dT& d, iT s, iT e) noexcept;

            template <typename dT, typename sT>
            static utfcvt_result cvtlen(dT& d, sT* s, size_t n) noexcept;

            template <typename dT, typename sT>
            static utfcvt_result cvtlen(dT& d, sT& s) noexcept;

            template <typename dT, typename sT>
            static utfcvt_result cvtlen(dT& d, sT* s) noexcept;

            /* cvt */

            template <typename dT, typename iT>
            static utfcvt_result cvt(dT& d, iT s, iT e);

            template <typename dT, typename sT>
            static utfcvt_result cvt(dT& d, sT* s, size_t n);

            template <typename dT, typename sT>
            static utfcvt_result cvt(dT& d, sT& s);

            template <typename dT, typename sT>
            static utfcvt_result cvt(dT& d, sT* s);
        };

        template <typename S>
        class utf_method : public S
        {
        protected:
            typedef S super;

        public:
            /* getcode */

            template <typename sT>
            static utfcvt_getcres getcode(sT* s, size_t n) noexcept;

            template <typename sT>
            static utfcvt_getcres getcode(sT& s) noexcept;

            template <typename sT>
            static utfcvt_getcres getcode(sT* s) noexcept;

            /* nextcode */

            template <typename iT>
            static utfcvt_getcres nextcode(iT& s, iT e) noexcept;

            template <typename sT>
            static utfcvt_getcres nextcode(sT*& s, size_t& n) noexcept;

            template <typename sT>
            static utfcvt_getcres nextcode(sT*& s) noexcept;

            /* to_utf8len */

            template <typename sT>
            static utfcvt_result to_utf8len(sT* s, size_t n) noexcept;

            template <typename sT>
            static utfcvt_result to_utf8len(sT& s) noexcept;

            template <typename sT>
            static utfcvt_result to_utf8len(sT* s) noexcept;

            /* to_utf16len */

            template <typename sT>
            static utfcvt_result to_utf16len(sT* s, size_t n) noexcept;

            template <typename sT>
            static utfcvt_result to_utf16len(sT& s) noexcept;

            template <typename sT>
            static utfcvt_result to_utf16len(sT* s) noexcept;

            /* to_utf32len */

            template <typename sT>
            static utfcvt_result to_utf32len(sT* s, size_t n) noexcept;

            template <typename sT>
            static utfcvt_result to_utf32len(sT& s) noexcept;

            template <typename sT>
            static utfcvt_result to_utf32len(sT* s) noexcept;

            /* to_utf8 */

            template <typename dT, typename sT>
            static utfcvt_result to_utf8(dT& d, sT* s, size_t n);

            template <typename dT, typename sT>
            static utfcvt_result to_utf8(dT& d, sT& s);

            template <typename dT, typename sT>
            static utfcvt_result to_utf8(dT& d, sT* s);

            /* to_utf16 */

            template <typename dT, typename sT>
            static utfcvt_result to_utf16(dT& d, sT* s, size_t n);

            template <typename dT, typename sT>
            static utfcvt_result to_utf16(dT& d, sT& s);

            template <typename dT, typename sT>
            static utfcvt_result to_utf16(dT& d, sT* s);

            /* to_utf32 */

            template <typename dT, typename iT>
            static utfcvt_result to_utf32(dT& d, iT* s, size_t n);

            template <typename dT, typename sT>
            static utfcvt_result to_utf32(dT& d, sT& s);

            template <typename dT, typename sT>
            static utfcvt_result to_utf32(dT& d, sT* s);
        };

        /**/

        template <typename T>
        struct utf_selector
        {
            typedef code_type<T> code_type;
            enum { size = code_type::size };

            template <int N> struct size_type;
            template <> struct size_type<1> { typedef utf8 type; };
            template <> struct size_type<2> { typedef utf16 type; };
            template <> struct size_type<4> { typedef utf32 type; };
            template <> struct size_type<8> { typedef utf32 type; };

            typedef typename size_type<size>::type utf_type;
        };

        template <typename T> struct utf_class : utf_selector<T>::utf_type {};

        template <typename dT>
        struct cvt_utf8
        {
            template <typename iT>
            static utfcvt_result cvtlen(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result cvt(dT& d, iT s, iT e);
        };

        template <typename dT>
        struct cvt_utf16
        {
            template <typename iT>
            static utfcvt_result cvtlen(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result cvt(dT& d, iT s, iT e);
        };

        template <typename dT>
        struct cvt_utf32
        {
            template <typename iT>
            static utfcvt_result cvtlen(iT s, iT e) noexcept;

            template <typename iT>
            static utfcvt_result cvt(dT& d, iT s, iT e);
        };

        template <typename dT>
        struct cvt_type
        {
            template <int N> struct size_type;
            template <> struct size_type<1> : cvt_utf8<dT> {};
            template <> struct size_type<2> : cvt_utf16<dT> {};
            template <> struct size_type<4> : cvt_utf32<dT> {};
            template <> struct size_type<8> : cvt_utf32<dT> {};
            typedef size_type<code_type<dT>::size> type;
        };

        template <typename dT> struct converter : cvt_type<dT>::type {};

    } // utfcvtimpl

    /*
     *
     */

    typedef utfcvtimpl::utf_method<utfcvtimpl::utf8> utf8;
    typedef utfcvtimpl::utf_method<utfcvtimpl::utf16> utf16;
    typedef utfcvtimpl::utf_method<utfcvtimpl::utf32> utf32;

    class utf : public utfcvtimpl::utf_method<utfcvtimpl::utf>
    {
    protected:
        typedef utfcvtimpl::utf_method<utfcvtimpl::utf> super;

    public:
        template <typename cT>
        static utfcvt_getcres getcode(const void* s, const void* e) noexcept;

        template <typename cT>
        static utfcvt_getcres nextcode(const void*& s, const void* e) noexcept;
    };

    /*
     *
     */

    template <typename T>
    class utfcvt_buffer
    {
    public:
        typedef T value_type;

        T *start;
        T *end;
        T *pointer;
        bool error;

    public:
        utfcvt_buffer(T* p, T* e) noexcept;
        utfcvt_buffer(T* p, size_t n) noexcept;

        void push_back(T c) noexcept;
    };

} // utfcvt

/*
 * Implementation
 */
namespace utfcvt
{
    /*
     *
     */

    inline utfcvt_constexpr std::uint8_t replace_u8c() noexcept
    {
        return 0x3f;  // '?'
    }

    inline utfcvt_constexpr std::uint16_t replace_u16c() noexcept
    {
        return 0xfffd;
    }

    inline utfcvt_constexpr std::uint32_t replace_u32c() noexcept
    {
        return replace_u16c();
    }

    /**/

    template <typename sT>
    inline size_t utflen(sT* s) noexcept
    {
        return utf::length(s);
    }

    /**/

    template <typename dT, typename iT>
    inline dT convert(iT s, iT e)
    {
        dT d;

        utf::cvt(d, s, e);
        return d;
    }

    template <typename dT, typename sT>
    inline dT convert(sT* s, size_t n)
    {
        dT d;

        utf::cvt(d, s, n);
        return d;
    }

    template <typename dT, typename sT>
    inline dT convert(sT& s)
    {
        return convert<dT>(s.begin(), s.end());
    }

    template <typename dT, typename sT>
    inline dT convert(sT* s)
    {
        dT d;

        utf::cvt(d, s, utf::length(s));
        return d;
    }

    /**/

    template <typename dT>
    inline dT convert(utfcvt_nextcode f, const void* s, const void* e)
    {
        typedef typename utfcvtimpl::code_type<dT>::char_t char_t;
        utf32_t replace = utf::replacement_character<char_t>();
        const void* p = s;
        dT d;

        while (p < e)
        {
            utfcvt_getcres res = f(p, e);

            utf::putcode(d, res.success ? res.code : replace);
        }
        return d;
    }

    template <typename dT>
    inline dT convert(utfcvt_nextcode f, const void* s, size_t n)
    {
        return convert<dT>(f, s, (const void*)((const uint8_t*)s + n));
    }

#if utfcvt_compiler_string

    template <typename sT>
    inline std::string to_string(sT s)
    {
        return convert<std::string>(s);
    }

    template <typename iT>
    inline std::string to_string(iT s, iT e)
    {
        return convert<std::string>(s, e);
    }

    template <typename sT>
    inline std::string to_string(sT* s, size_t n)
    {
        return convert<std::string>(s, n);
    }

#endif /* utfcvt_compiler_string */
#if utfcvt_compiler_wstring

    template <typename sT>
    inline std::wstring to_wstring(sT s)
    {
        return convert<std::wstring>(s);
    }

    template <typename iT>
    inline std::wstring to_wstring(iT s, iT e)
    {
        return convert<std::wstring>(s, e);
    }

    template <typename sT>
    inline std::wstring to_wstring(sT* s, size_t n)
    {
        return convert<std::wstring>(s, n);
    }

#endif /* utfcvt_compiler_wstring */
#if utfcvt_compiler_u8string

    template <typename sT>
    inline std::u8string to_u8string(sT s)
    {
        return convert<std::u8string>(s);
    }

    template <typename iT>
    inline std::u8string to_u8string(iT s, iT e)
    {
        return convert<std::u8string>(s, e);
    }

    template <typename sT>
    inline std::u8string to_u8string(sT* s, size_t n)
    {
        return convert<std::u8string>(s, n);
    }

#endif /* utfcvt_compiler_u8string */
#if utfcvt_compiler_u16string

    template <typename sT>
    inline std::u16string to_u16string(sT s)
    {
        return convert<std::u16string>(s);
    }

    template <typename iT>
    inline std::u16string to_u16string(iT s, iT e)
    {
        return convert<std::u16string>(s, e);
    }

    template <typename sT>
    inline std::u16string to_u16string(sT* s, size_t n)
    {
        return convert<std::u16string>(s, n);
    }

#endif /* utfcvt_compiler_u16string */
#if utfcvt_compiler_u32string

    template <typename sT>
    inline std::u32string to_u32string(sT s)
    {
        return convert<std::u32string>(s);
    }

    template <typename iT>
    inline std::u32string to_u32string(iT s, iT e)
    {
        return convert<std::u32string>(s, e);
    }

    template <typename sT>
    inline std::u32string to_u32string(sT* s, size_t n)
    {
        return convert<std::u32string>(s, n);
    }

#endif /* utfcvt_compiler_u32string */

    /*
     * utfcvt_getcres
     */

    inline utfcvt_getcres::utfcvt_getcres() noexcept
        : success(false)
        , size(0)
        , code(0)
    {
        /*NOOP*/
    }

    inline utfcvt_getcres::utfcvt_getcres(size_t s, utf32_t c, bool b) noexcept
        : success(b)
        , size(std::uint8_t(s))
        , code(c)
    {
        /*NOOP*/
    }

    /*
     * utfcvt_result
     */

    inline utfcvt_result::utfcvt_result() noexcept
        : success(false)
        , read(0)
        , write(0)
    {
        /*NOOP*/
    }

    inline utfcvt_result::utfcvt_result(size_t r, size_t w, bool b) noexcept
        : success(b)
        , read(r)
        , write(w)
    {
        /*NOOP*/
    }

    /*
     * utf
     */

    template <typename cT>
    inline utfcvt_getcres utf::getcode(const void* s, const void* e) noexcept
    {
        return super::getcode<const cT*>((const cT*)s, (const cT*)e);
    }

    template <typename cT>
    inline utfcvt_getcres utf::nextcode(const void*& s, const void* e) noexcept
    {
        return super::nextcode<const cT*>((const cT*&)s, (const cT*)e);
    }

    /*
     * utfcvt_buffer
     */

    template <typename T>
    inline utfcvt_buffer<T>::utfcvt_buffer(T* p, T* e) noexcept
        : start(p)
        , end(e)
        , pointer(p)
        , error(false)
    {
        /*NOOP*/
    }

    template <typename T>
    inline utfcvt_buffer<T>::utfcvt_buffer(T* p, size_t n) noexcept
        : start(p)
        , end(p + n)
        , pointer(p)
        , error(false)
    {
        /*NOOP*/
    }

    template <typename T>
    inline void utfcvt_buffer<T>::push_back(T c) noexcept
    {
        error = (pointer >= end);
        if (!error)
            *pointer++ = c;
    }

    /*
     * utfcvtimpl
     */

    namespace utfcvtimpl
    {
        /*
         * bsr
         */

        template <typename T>
        inline utfcvt_constexpr int bsr32(T x) noexcept
        {
#if utfcvt_compiler_bsr_msvc
            unsigned long r;
            return _BitScanReverse(&r, std::uint32_t(x)) ? int(r) : -1;
#elif utfcvt_compiler_builtin_clz
            return !std::uint32_t(x) ? -1 : 31 ^ __builtin_clz(x);
#elif utfcvt_compiler_clz
            return 31 - clz32(std::uint32_t(x));
#else /* C++ */
            return 31 - clzN(std::uint32_t(x));
#endif
        }

        /*
         * clz
         */

        template <typename T>
        inline utfcvt_constexpr int clzNstep(T x, int r, int b) noexcept
        {
            return (!b ? r - int(x) : (x >> b) ?
                    clzNstep(x >> b, r + 0, b >> 1) :
                    clzNstep(x >> 0, r + b, b >> 1));
        }
        template <typename T>
        utfcvt_constexpr int clzN(T x) noexcept
        {
            return clzNstep(x, 1, sizeof(T) << 2);
        }

        template <typename T>
        inline utfcvt_constexpr int clz32(T x) noexcept
        {
#if utfcvt_compiler_countl_zero
            return std::countl_zero(std::uint32_t(x));
#elif utfcvt_compiler_builtin_clz
            return !std::uint32_t(x) ? 32 : __builtin_clz(x);
#elif utfcvt_compiler_bsr
            return 31 - bsr32(x);
#else /* C++ */
            return clzN(std::uint32_t(x));
#endif
        }

        /*
         * utf_common
         */

        template <typename sT>
        inline size_t utf_common::len(sT* s) noexcept
        {
            return length(s);
        }

        template <typename sT>
        inline size_t utf_common::length(sT* s) noexcept
        {
            sT* p = s;

            while (*p)
                ++p;
            return p - s;
        }

        template <typename dU, typename sU, typename dT, typename iT>
        inline utfcvt_result utf_common::append(dT& d, iT s, iT e)
        {
            typedef utfcvtimpl::code_type<dT> code_dtype;
            typedef utfcvtimpl::code_type<iT> code_stype;

            if (int(dU::code_size) != int(sU::code_size))
                return convert<dU, sU>(d, s, e);
            if (int(code_dtype::size) < int(code_stype::size))
                return degrade<dU>(d, s, e);
            return copy(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf_common::copy(dT& d, iT s, iT e)
        {
            typedef typename dT::value_type push_t;
            typedef typename data_type<push_t>::value_type push_value;
            size_t n = e - s;

            for (iT p = s; p != e; ++p)
            {
                push_t v;

                d.push_back((v = push_value(*p)));
            }
            return utfcvt_result(n, n);
        }

        template <typename dU, typename dT, typename iT>
        inline utfcvt_result utf_common::degrade(dT& d, iT s, iT e)
        {
            typedef typename dT::value_type push_t;
            typedef typename data_type<push_t>::value_type push_value;
            typedef typename utfcvtimpl::code_type<dT>::char_t dchar_t;
            typedef typename utfcvtimpl::code_type<iT>::char_t schar_t;

            schar_t dmax = schar_t((std::numeric_limits<dchar_t>::max)());
            size_t n = e - s;
            bool b = true;

            for (iT p = s; p != e; ++p)
            {
                schar_t c = *p;

                if (c > dmax)
                {
                    dchar_t r = dU::replacement_character();

                    c = schar_t(sizeof(push_t) < 2 ? replace_u8c() : r);
                    b = false;
                }
                {
                    push_t v;

                    d.push_back((v = push_value(c)));
                }
            }
            return utfcvt_result(n, n, b);
        }

        template <typename dU, typename sU, typename dT, typename iT>
        utfcvt_result utf_common::convert(dT& d, iT s, iT e)
        {
            size_t r = 0, w = 0;
            bool b = true;

            for (iT p = s; p != e;)
            {
                utfcvt_getcres res = sU::getcode(p, e);
                utf32_t code = res.success ? res.code : dU::replacement_character();

                b &= res.success;
                r += res.size;
                p += res.size;

                w += dU::putcode(d, code);
            }
            return utfcvt_result(r, w, b);
        }

        template <typename dU, typename sU, typename iT>
        inline utfcvt_result utf_common::count(iT s, iT e) noexcept
        {
            if (int(dU::code_size) == int(sU::code_size))
                return utfcvt_result(e - s, e - s);
            return cvtlen<dU, sU>(s, e);
        }

        template <typename dU, typename sU, typename iT>
        utfcvt_result utf_common::cvtlen(iT s, iT e) noexcept
        {
            size_t r = 0, w = 0;
            bool b = true;

            for (iT p = s; p != e;)
            {
                utfcvt_getcres res = sU::getcode(p, e);
                utf32_t code = res.success ? res.code : dU::replacement_character();

                b &= res.success;
                r += res.size;
                p += res.size;

                w += (std::max)(size_t(1), dU::putcodelen(code));
            }
            return utfcvt_result(r, w, b);
        }

        /*
         * utf8
         */

        inline utfcvt_constexpr std::uint8_t utf8::replacement_character() noexcept
        {
            return replace_u8c();
        }

#if !(utfcvt_compiler_bsr || utfcvt_compiler_clz)

        template <typename cT>
        utfcvt_constexpr size_t utf8::getcodelen(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

            return ((char_t(c) < 0x80) ? 1 :
                    (char_t(c) < 0xc0) ? 0 /* error */ :
                    (char_t(c) < 0xe0) ? 2 :
                    (char_t(c) < 0xf0) ? 3 :
                    (char_t(c) < 0xf8) ? 4 :
                    (char_t(c) < 0xfc) ? 5 :
                    (char_t(c) < 0xfe) ? 6 :
                    0 /* error */);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::getcodelen32(cT c) noexcept
        {
            return getcodelen(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::getcodelen64(cT c) noexcept
        {
            return getcodelen(c);
        }

#else  /* utfcvt_compiler_(bsr||clz) */

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::getcodelen(cT c) noexcept
        {
            return sizeof(cT) <= 4 ? getcodelen32(c) : getcodelen64(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::getcodelen32(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

#if utfcvt_compiler_bsr
            return ((char_t(c) < 0x80) ? 1 :
                    (char_t(c) > 0xff) ? 0 /* error */ :
                    (06543201 >> ((7 - utfcvtimpl::bsr32(char_t(c) ^ 0xff)) * 3) & 7));
#elif utfcvt_compiler_clz
            return ((char_t(c) < 0x80) ? 1 :
                    (char_t(c) > 0xff) ? 0 /* error */ :
                    (06543201 >> ((utfcvtimpl::clz32(char_t(c) ^ 0xff) - 24) * 3) & 7));
#endif
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::getcodelen64(cT c) noexcept
        {
            return !(std::uint64_t(c) >> 32) ? getcodelen32(utf32_t(c)) : 0;
        }

#endif /* utfcvt_compiler_(bsr||clz) */

        template <typename iT>
        utfcvt_getcres utf8::getcode(iT s, iT e) noexcept
        {
            typedef typename utfcvtimpl::code_type<iT>::char_t char_t;

            iT p = s;

            if (p == e)
                return utfcvt_getcres(0, 0);

            char_t c = *p++;
            size_t l = getcodelen(c);

            if (l < 2)
                return utfcvt_getcres(1, c, l != 0);

            utf32_t w = c & (0x7f >> l);

            size_t t = 1;
            while (p != e)
            {
                c = *p++;
                if (!(0x80 <= c && c <= 0xbf))
                    break;
                w = (w << 6) | (c & 0x3f);
                ++t;
            }
            return utfcvt_getcres(t, w, l == t);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::putcodelen(cT c) noexcept
        {
            return sizeof(cT) <= 4 ? putcodelen32(c) : putcodelen64(c);
        }

#if utfcvt_utf8_putcodelen_mode == 0

        template <typename cT>
        utfcvt_constexpr size_t utf8::putcodelen32(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;

            return ((comp_t(char_t(c)) < (1U << +7)) ? 1 :
                    (comp_t(char_t(c)) < (1U << 11)) ? 2 :
                    (comp_t(char_t(c)) < (1U << 16)) ? 3 :
                    (comp_t(char_t(c)) < (1U << 21)) ? 4 :
                    (comp_t(char_t(c)) < (1U << 26)) ? 5 :
                    (comp_t(char_t(c)) < (1U << 31)) ? 6 :
                    0 /* error */);
        }

#elif utfcvt_utf8_putcodelen_mode == 1

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::putcodelen32(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

            // int n = utfcvtimpl::clz32(char_t(c));
            // if (!n--) return 0;  // error
            // // 11111111 2222 3333 3 444 44 55 555 6 6666
            // // 11111111 0000 1111/1 000/00 11/111 0/0000 = FF0F83E0
            // // 00000000 1111 1111/1 000/00 00/000 1/1111 = 00FF801F
            // // 00000000 0000 0000/0 111/11 11/111 1/1111 = 00007FFF
            // std::uint_least32_t b0 = 1 & (0xFF0F83E0 >> n);
            // std::uint_least32_t b1 = 1 & (0x00FF801F >> n);
            // std::uint_least32_t b2 = 1 & (0x00007FFF >> n);
            // return size_t((b0 << 0) | (b1 << 1) | (b2 << 2));

            return !utfcvtimpl::clz32(char_t(c)) ? 0 /* error */ :
                ((1 & (0xFF0F83E0 >> (utfcvtimpl::clz32(char_t(c)) - 1))) << 0) |
                ((1 & (0x00FF801F >> (utfcvtimpl::clz32(char_t(c)) - 1))) << 1) |
                ((1 & (0x00007FFF >> (utfcvtimpl::clz32(char_t(c)) - 1))) << 2);
        }

#elif utfcvt_utf8_putcodelen_mode == 2

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::putcodelen32(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

            static const std::uint8_t table[32+1] = {
                0,
                6, 6, 6, 6, 6,
                5, 5, 5, 5, 5,
                4, 4, 4, 4, 4,
                3, 3, 3, 3, 3,
                2, 2, 2, 2,
                1, 1, 1, 1, 1, 1, 1,
                1,
            };

            return table[utfcvtimpl::clz32(char_t(c))];
        }

#endif /* utfcvt_utf8_putcodelen_mode */

        template <typename cT>
        inline utfcvt_constexpr size_t utf8::putcodelen64(cT c) noexcept
        {
            return !(std::uint64_t(c) >> 32) ? putcodelen32(utf32_t(c)) : 0;
        }

        /* putcode */

        template <typename dT, typename cT>
        size_t utf8::putcode(dT& d, cT c_)
        {
            typedef typename dT::value_type push_t;
            typedef typename data_type<push_t>::value_type push_value;
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

            char_t c = char_t(c_);
            size_t l = utf8::putcodelen(c);

            int b = int(l) - 1;

            if (b < 1)
            {
                if (b < 0)
                    c = utf8::replacement_character();
                {
                    push_t v;

                    d.push_back((v = push_value(c)));
                }
                return 1;
            }

            int s = b * 6;
            {
                push_t v;

                v = push_value((0xff & (int(~0x7f) >> b)) |
                               ((c >> s) & (0x3f >> b)));
                d.push_back(v);
            }
            do
            {
                push_t v;

                s -= 6;
                d.push_back((v = push_value(0x80 | ((c >> s) & 0x3f))));
            } while (s > 0);

            return l;
        }

        template <typename iT>
        inline utfcvt_result utf8::to_utf8len(iT s, iT e) noexcept
        {
            return count<utf8, utf8>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf8::to_utf16len(iT s, iT e) noexcept
        {
            return count<utf16, utf8>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf8::to_utf32len(iT s, iT e) noexcept
        {
            return count<utf32, utf8>(s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf8::to_utf8(dT& d, iT s, iT e)
        {
            return append<utf8, utf8>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf8::to_utf16(dT& d, iT s, iT e)
        {
            return append<utf16, utf8>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf8::to_utf32(dT& d, iT s, iT e)
        {
            return append<utf32, utf8>(d, s, e);
        }

        /*
         * utf16
         */

        inline utfcvt_constexpr std::uint16_t utf16::replacement_character() noexcept
        {
            return replace_u16c();
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::getcodelen(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;

            return ((comp_t(char_t(c)) < 0x0d800) ? 1 :
                    (comp_t(char_t(c)) < 0x0dc00) ? 2 :
                    (comp_t(char_t(c)) < 0x0e000) ? 0 /* error */ :
                    (comp_t(char_t(c)) < 0x10000) ? 1 :
                    0 /* error */);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::getcodelen32(cT c) noexcept
        {
            return getcodelen(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::getcodelen64(cT c) noexcept
        {
            return getcodelen(c);
        }

        template <typename iT>
        inline utfcvt_getcres utf16::getcode(iT s, iT e) noexcept
        {
            typedef typename utfcvtimpl::code_type<iT>::char_t char_t;

            iT p = s;

            if ((e - p) <= 0)
                return utfcvt_getcres(0, 0);

            char_t c = *p++;
            size_t l = getcodelen(c);

            switch (l)
            {
            default: return utfcvt_getcres(1, c, false);
            case 1: return utfcvt_getcres(1, c);
            case 2: break;
            }

            if ((e - p) <= 0)
                return utfcvt_getcres(1, c, false);

            char_t d = *p;

            if (!(0xdc00 <= d && d <= 0xdfff))
                return utfcvt_getcres(1, c, false);

            utf32_t u = 0x10000 + ((c & 0x03ff) << 10) + (d & 0x03ff);

            return utfcvt_getcres(2, u);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::putcodelen(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;

            return ((comp_t(char_t(c)) < 0x010000) ? 1 :
                    (comp_t(char_t(c)) < 0x110000) ? 2 :
                    0 /* error */);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::putcodelen32(cT c) noexcept
        {
            return putcodelen(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf16::putcodelen64(cT c) noexcept
        {
            return putcodelen(c);
        }

        template <typename dT, typename cT>
        inline size_t utf16::putcode(dT& d, cT c_)
        {
            typedef typename dT::value_type push_t;
            typedef typename data_type<push_t>::value_type push_value;
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;

            char_t c = char_t(c_);

            if (int(utfcvtimpl::data_type<dT>::size) < 2)
            {
                if (c > 0xff)
                    c = utf8::replacement_character();
                {
                    push_t v;

                    d.push_back((v = push_value(c)));
                }
                return 1;
            }

            size_t l = utf16::putcodelen(c);

            if (l < 2)
            {
                if (!l)
                    c = char_t(utf16::replacement_character());
                {
                    push_t v;

                    d.push_back((v = push_value(c)));
                }
                return 1;
            }
            else
            {
                push_t v;

                d.push_back((v = push_value(0xd800 | ((c - 0x10000) >> 10))));
                d.push_back((v = push_value(0xdc00 | (c & 0x03ff))));
                return 2;
            }
        }

        template <typename iT>
        inline utfcvt_result utf16::to_utf8len(iT s, iT e) noexcept
        {
            return count<utf8, utf16>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf16::to_utf16len(iT s, iT e) noexcept
        {
            return count<utf16, utf16>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf16::to_utf32len(iT s, iT e) noexcept
        {
            return count<utf32, utf16>(s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf16::to_utf8(dT& d, iT s, iT e)
        {
            return append<utf8, utf16>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf16::to_utf16(dT& d, iT s, iT e)
        {
            return append<utf16, utf16>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf16::to_utf32(dT& d, iT s, iT e)
        {
            return append<utf32, utf16>(d, s, e);
        }

        /*
         * utf32
         */

        inline utfcvt_constexpr std::uint32_t utf32::replacement_character() noexcept
        {
            return replace_u32c();
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::getcodelen(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;
            return comp_t(char_t(c)) <= UINT32_C(0xffffffff) ? 1 : 0;
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::getcodelen32(cT c) noexcept
        {
            return getcodelen(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::getcodelen64(cT c) noexcept
        {
            return getcodelen(c);
        }

        template <typename iT>
        inline utfcvt_getcres utf32::getcode(iT s, iT e) noexcept
        {
            typedef typename utfcvtimpl::code_type<iT>::char_t char_t;

            if ((e - s) <= 0)
                return utfcvt_getcres(0, 0);

            char_t c = char_t(*s);

            return utfcvt_getcres(1, c, getcodelen(c) != 0);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::putcodelen(cT c) noexcept
        {
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;

            return comp_t(char_t(c)) <= UINT32_C(0xffffffff) ? 1 : 0;
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::putcodelen32(cT c) noexcept
        {
            return putcodelen(c);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf32::putcodelen64(cT c) noexcept
        {
            return putcodelen(c);
        }

        template <typename dT, typename cT>
        inline size_t utf32::putcode(dT& d, cT c_)
        {
            typedef typename dT::value_type push_t;
            typedef typename data_type<push_t>::value_type push_value;
            typedef typename utfcvtimpl::code_type<cT>::char_t char_t;
            typedef typename utfcvtimpl::code_type<cT>::comp_t comp_t;

            comp_t c = char_t(c_);

            switch (int(utfcvtimpl::code_type<dT>::size))
            {
            case 1:
                if (c > 0xff)
                    c = utf8::replacement_character();
                break;
            case 2:
                if (c > 0xffff)
                    c = utf16::replacement_character();
                break;
            default:
                if (!putcodelen(c))
                    c = utf32::replacement_character();
                break;
            }
            {
                push_t v;

                d.push_back((v = push_value(c)));
            }
            return 1;
        }

        template <typename iT>
        inline utfcvt_result utf32::to_utf8len(iT s, iT e) noexcept
        {
            return count<utf8, utf32>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf32::to_utf16len(iT s, iT e) noexcept
        {
            return count<utf16, utf32>(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf32::to_utf32len(iT s, iT e) noexcept
        {
            return count<utf32, utf32>(s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf32::to_utf8(dT& d, iT s, iT e)
        {
            return append<utf8, utf32>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf32::to_utf16(dT& d, iT s, iT e)
        {
            return append<utf16, utf32>(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf32::to_utf32(dT& d, iT s, iT e)
        {
            return append<utf32, utf32>(d, s, e);
        }

        /*
         * utf
         */

        template <typename cT>
        inline utfcvt_constexpr cT utf::replacement_character() noexcept
        {
            return utfcvtimpl::utf_class<cT>::replacement_character();
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf::getcodelen(cT c) noexcept
        {
            return utfcvtimpl::utf_class<cT>::getcodelen(c);
        }

        template <typename iT>
        inline utfcvt_getcres utf::getcode(iT s, iT e) noexcept
        {
            return utfcvtimpl::utf_class<iT>::getcode(s, e);
        }

        template <typename iT>
        inline utfcvt_getcres utf::nextcode(iT& s, iT e) noexcept
        {
            return utfcvtimpl::utf_class<iT>::nextcode(s, e);
        }

        template <typename cT>
        inline utfcvt_constexpr size_t utf::putcodelen(cT c) noexcept
        {
            return utfcvtimpl::utf_class<cT>::putcodelen(c);
        }

        template <typename dT, typename cT>
        inline size_t utf::putcode(dT& d, cT c)
        {
            return utfcvtimpl::utf_class<dT>::putcode(d, c);
        }

        template <typename iT>
        inline utfcvt_result utf::to_utf8len(iT s, iT e) noexcept
        {
            return utfcvtimpl::utf_class<iT>::to_utf8len(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf::to_utf16len(iT s, iT e) noexcept
        {
            return utfcvtimpl::utf_class<iT>::to_utf16len(s, e);
        }

        template <typename iT>
        inline utfcvt_result utf::to_utf32len(iT s, iT e) noexcept
        {
            return utfcvtimpl::utf_class<iT>::to_utf32len(s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf::to_utf8(dT& d, iT s, iT e)
        {
            return utfcvtimpl::utf_class<iT>::to_utf8(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf::to_utf16(dT& d, iT s, iT e)
        {
            return utfcvtimpl::utf_class<iT>::to_utf16(d, s, e);
        }

        template <typename dT, typename iT>
        inline utfcvt_result utf::to_utf32(dT& d, iT s, iT e)
        {
            return utfcvtimpl::utf_class<iT>::to_utf32(d, s, e);
        }

        /* cvtlen */

        template <typename dT, typename iT>
        inline utfcvt_result utf::cvtlen(dT& d, iT s, iT e) noexcept
        {
            (void)d;
            return utfcvtimpl::converter<dT>::cvtlen(s, e);
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvtlen(dT& d, sT* s, size_t n) noexcept
        {
            return cvtlen(d, s, s + n);
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvtlen(dT& d, sT& s) noexcept
        {
            return cvtlen(d, s.begin(), s.end());
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvtlen(dT& d, sT* s) noexcept
        {
            return cvtlen(d, s, length(s));
        }

        /* cvt */

        template <typename dT, typename iT>
        inline utfcvt_result utf::cvt(dT& d, iT s, iT e)
        {
            return utfcvtimpl::converter<dT>::cvt(d, s, e);
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvt(dT& d, sT* s, size_t n)
        {
            return cvt(d, s, s + n);
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvt(dT& d, sT& s)
        {
            return cvt(d, s.begin(), s.end());
        }

        template <typename dT, typename sT>
        inline utfcvt_result utf::cvt(dT& d, sT* s)
        {
            return cvt(d, s, length(s));
        }

        /*
         * utf_method
         */

        /* getcode */

        template <typename S>
        template <typename sT>
        inline utfcvt_getcres utf_method<S>::getcode(sT* s, size_t n) noexcept
        {
            return super::getcode(s, s + n);
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_getcres utf_method<S>::getcode(sT& s) noexcept
        {
            return getcode(s.begin(), s.end());
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_getcres utf_method<S>::getcode(sT* s) noexcept
        {
            return getcode(s, utf::length(s));
        }

        /* nextcode */

        template <typename S>
        template <typename iT>
        inline utfcvt_getcres utf_method<S>::nextcode(iT& s, iT e) noexcept
        {
            utfcvt_getcres res = super::getcode(s, e);

            s += res.size;
            return res;
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_getcres utf_method<S>::nextcode(sT*& s, size_t& n) noexcept
        {
            utfcvt_getcres res = super::getcode(s, s + n);

            s += res.size;
            n -= res.size;
            return res;
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_getcres utf_method<S>::nextcode(sT*& s) noexcept
        {
            size_t n = utf::length(s);
            return nextcode(s, n);
        }

        /* to_utf8len */

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf8len(sT* s, size_t n) noexcept
        {
            return super::to_utf8len(s, s + n);
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf8len(sT& s) noexcept
        {
            return to_utf8len(s.begin(), s.end());
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf8len(sT* s) noexcept
        {
            return to_utf8len(s, utf::length(s));
        }

        /* to_utf16len */

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf16len(sT* s, size_t n) noexcept
        {
            return super::to_utf16len(s, s + n);
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf16len(sT& s) noexcept
        {
            return to_utf16len(s.begin(), s.end());
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf16len(sT* s) noexcept
        {
            return to_utf16len(s, utf::length(s));
        }

        /* to_utf32len */

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf32len(sT* s, size_t n) noexcept
        {
            return super::to_utf32len(s, s + n);
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf32len(sT& s) noexcept
        {
            return to_utf32len(s.begin(), s.end());
        }

        template <typename S>
        template <typename sT>
        inline utfcvt_result utf_method<S>::to_utf32len(sT* s) noexcept
        {
            return to_utf32len(s, utf::length(s));
        }

        /* to_utf8 */

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf8(dT& d, sT* s, size_t n)
        {
            return super::to_utf8(d, s, s + n);
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf8(dT& d, sT& s)
        {
            return to_utf8(d, s.begin(), s.end());
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf8(dT& d, sT* s)
        {
            return to_utf8(d, s, utf::length(s));
        }

        /* to_utf16 */

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf16(dT& d, sT* s, size_t n)
        {
            return super::to_utf16(d, s, s + n);
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf16(dT& d, sT& s)
        {
            return to_utf16(d, s.begin(), s.end());
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf16(dT& d, sT* s)
        {
            return to_utf16(d, s, utf::length(s));
        }

        /* to_utf32 */

        template <typename S>
        template <typename dT, typename iT>
        inline utfcvt_result utf_method<S>::to_utf32(dT& d, iT* s, size_t n)
        {
            return super::to_utf32(d, s, s + n);
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf32(dT& d, sT& s)
        {
            return to_utf32(d, s.begin(), s.end());
        }

        template <typename S>
        template <typename dT, typename sT>
        inline utfcvt_result utf_method<S>::to_utf32(dT& d, sT* s)
        {
            return to_utf32(d, s, utf::length(s));
        }

        /*
         * cvt_utf8
         */

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf8<dT>::cvtlen(iT s, iT e) noexcept
        {
            return utf_class<iT>::to_utf8len(s, e);
        }

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf8<dT>::cvt(dT& d, iT s, iT e)
        {
            return utf_class<iT>::to_utf8(d, s, e);
        }

        /*
         * cvt_utf16
         */

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf16<dT>::cvtlen(iT s, iT e) noexcept
        {
            return utf_class<iT>::to_utf16len(s, e);
        }

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf16<dT>::cvt(dT& d, iT s, iT e)
        {
            return utf_class<iT>::to_utf16(d, s, e);
        }

        /*
         * cvt_utf32
         */

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf32<dT>::cvtlen(iT s, iT e) noexcept
        {
            return utf_class<iT>::to_utf32len(s, e);
        }

        template <typename dT>
        template <typename iT>
        inline utfcvt_result cvt_utf32<dT>::cvt(dT& d, iT s, iT e)
        {
            return utf_class<iT>::to_utf32(d, s, e);
        }

    } //utfcvtimpl
} // utfcvt

#ifdef utfcvt_defined_noexcept
#undef utfcvt_defined_noexcept
#undef noexcept
#endif

///////////////////
// Local Variables:
// c-basic-offset: 4
// indent-tabs-mode: nil
// tab-width: 4
// End:
///////
