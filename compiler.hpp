/*
 * compiler.hpp
 */
#pragma once

/*
 *
 */

#ifndef WASMGEN_COMPILER_LANG
#  if defined(_MSVC_LANG) && _MSVC_LANG > __cplusplus
#    define WASMGEN_COMPILER_LANG  _MSVC_LANG
#  else
#    define WASMGEN_COMPILER_LANG  __cplusplus
#  endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#define WASMGEN_TARGET_UNIX     0
#define WASMGEN_TARGET_WINDOWS  1
#else
#define WASMGEN_TARGET_UNIX     1
#define WASMGEN_TARGET_WINDOWS  0
#endif
#ifndef NOMINMAX
#define NOMINMAX 1
#endif

#if WASMGEN_COMPILER_LANG < 201103L
#define WASMGEN_COMPILER_CXX11  0
#else /* C++11 */
#define WASMGEN_COMPILER_CXX11  1
#endif

#if WASMGEN_COMPILER_LANG < 201402L
#define WASMGEN_COMPILER_CXX14  0
#else /* C++14 */
#define WASMGEN_COMPILER_CXX14  1
#endif

#if WASMGEN_COMPILER_LANG < 201703L
#define WASMGEN_COMPILER_CXX17  0
#else /* C++17 */
#define WASMGEN_COMPILER_CXX17  1
#endif

#if WASMGEN_COMPILER_LANG < 202002L
#define WASMGEN_COMPILER_CXX20  0
#else  /* C++20 */
#define WASMGEN_COMPILER_CXX20  1
#endif

#define WASMGEN_COMPILER_CXX23  0

/*
 *
 */

#ifndef WASMGEN_USE_THREAD
#define WASMGEN_USE_THREAD  0
#endif

/*
 *
 */

#include <cassert>
#include <cctype>
#include <climits>
#include <clocale>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cwchar>

#include <algorithm>
#include <atomic>
#include <exception>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if WASMGEN_COMPILER_CXX20
#include <bit>
#include <compare>
#endif

#if WASMGEN_TARGET_UNIX
#include <unistd.h>
#endif
#if WASMGEN_TARGET_WINDOWS
#include <direct.h>
#include <intrin.h>
#endif

/*
 *
 */

#ifndef fallthrough
#  if WASMGEN_COMPILER_CXX17
#    define fallthrough  [[fallthrough]]
#  else
#    define fallthrough  /*FALLTHRU*/
#  endif
#endif

/*
 * __builtin_<function>
 */

#ifndef __has_builtin
#define __has_builtin(f)  0
#endif

#ifndef WASMGEN_COMPILER_BUILTIN_CLZ
#  if __has_builtin(__builtin_clz)
#    define WASMGEN_COMPILER_BUILTIN_CLZ  1
#  else
#    define WASMGEN_COMPILER_BUILTIN_CLZ  0
#  endif
#endif
#ifndef WASMGEN_COMPILER_BUILTIN_CLZLL
#  if __has_builtin(__builtin_clzll)
#    define WASMGEN_COMPILER_BUILTIN_CLZLL  1
#  else
#    define WASMGEN_COMPILER_BUILTIN_CLZLL  0
#  endif
#endif
#ifndef WASMGEN_COMPILER_COUNTL_ZERO
#  if __cpp_lib_bitops < 201907L
#    define WASMGEN_COMPILER_COUNTL_ZERO  0
#  else
#    define WASMGEN_COMPILER_COUNTL_ZERO  1
#  endif
#endif

/*
 * Endian
 */

#if !(defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && defined(__ORDER_LITTLE_ENDIAN__))
#  if defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64) || defined(_M_ARM)
#    define __ORDER_BIG_ENDIAN__     4321
#    define __ORDER_LITTLE_ENDIAN__  1234
#    define __BYTE_ORDER__           __ORDER_LITTLE_ENDIAN__
#  else
#    error "unknown compiler"
#  endif
#endif

#if !WASMGEN_COMPILER_CXX20 && __cpp_lib_endian < 201907L
#define WASMGEN_COMPILER_ENDIAN  0
#else
#define WASMGEN_COMPILER_ENDIAN  1
#endif

#if !WASMGEN_COMPILER_CXX23 && __cpp_lib_byteswap < 202110L
#define WASMGEN_COMPILER_BYTESWAP  0
#else
#define WASMGEN_COMPILER_BYTESWAP  1
#endif

/*
 * char / wchar_t / char{8|16|32}_t
 */

#define WASMGEN_COMPILER_CXX_CHAR  1
#define WASMGEN_COMPILER_CXX_WCHAR  1

#if __cpp_char8_t >= 201811L
#  define WASMGEN_COMPILER_CXX_CHAR8  1
#else
#  define WASMGEN_COMPILER_CXX_CHAR8  0
#endif

#if (WASMGEN_COMPILER_CXX11 ||                   \
     (defined(_MSC_VER) && _MSC_VER >= 1900) || \
     defined(__clang__) ||                      \
     0)
#  define WASMGEN_COMPILER_CXX_CHAR16  1
#  define WASMGEN_COMPILER_CXX_CHAR32  1
#else
#  define WASMGEN_COMPILER_CXX_CHAR16  0
#  define WASMGEN_COMPILER_CXX_CHAR32  0
#endif

#ifndef WASMGEN_COMPILER_CHAR
#  define WASMGEN_COMPILER_CHAR  WASMGEN_COMPILER_CXX_CHAR
#endif
#ifndef WASMGEN_COMPILER_WCHAR
#  define WASMGEN_COMPILER_WCHAR  WASMGEN_COMPILER_CXX_WCHAR
#endif
#ifndef WASMGEN_COMPILER_CHAR8
#  define WASMGEN_COMPILER_CHAR8  WASMGEN_COMPILER_CXX_CHAR8
#endif
#ifndef WASMGEN_COMPILER_CHAR16
#  define WASMGEN_COMPILER_CHAR16  WASMGEN_COMPILER_CXX_CHAR16
#endif
#ifndef WASMGEN_COMPILER_CHAR32
#  define WASMGEN_COMPILER_CHAR32  WASMGEN_COMPILER_CXX_CHAR32
#endif
