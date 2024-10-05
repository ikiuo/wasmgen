/*
 * option.hpp
 */
#pragma once
#include "utility.hpp"

namespace wasmgen
{
    /**/

    extern const char* program;
    extern int flag_debug;
    extern bool flag_quiet;
    extern bool flag_verbose;

    /**/

    StdString hexstr(const void* p, size_t n);

    void debug(const char* file, int line, const StringBlock& msg);

    void message(const char* msg);
    void message(const StringBlock& msg);
    template <typename... Args> void message(Args... args);

    void dumpbin(const void* addr, size_t size, size_t indent = 0);
}

/*
 *
 */

namespace wasmgen
{

#define WASMGEN_DEBUG(n, ...)  {                        \
        if (flag_debug >= (n))                          \
            debug(__FILE__, __LINE__, {__VA_ARGS__});   \
    }

#if !defined(NDBEUG)
#  ifdef __GNUC__
#    define WASMGEN_DEBUG_THIS(n, C)  WASMGEN_DEBUG(n, __PRETTY_FUNCTION__, " [this=", this, "]\n")
#  else
#    define WASMGEN_DEBUG_THIS(n, C)  WASMGEN_DEBUG(n, #C "::", __func__, " [this=", this, "]\n")
#  endif
#  define WASMGEN_XDEBUG  WASMGEN_DEBUG
#else
#  define WASMGEN_DEBUG_THIS(n, C)  /*NOOP*/
#  define WASMGEN_XDEBUG(...)       /*NOOP*/
#endif

#define verbose(...)  { if (flag_verbose) message(__VA_ARGS__); }

    /**/

    inline int hexchr(int n)
    {
        n &= 15;
        return n < 10 ? n + '0' : n - 10 + 'A';
    }

    inline void debug(const char* file, int line, const StringBlock& msg)
    {
        message(file, ":", std::setw(4), std::setfill('0'), line, ": ", msg);
    }

    template <typename... Args>
    inline void message(Args... args)
    {
        message(StringBlock{args...});
    }

    inline void message(const char* msg)
    {
        if (!flag_quiet)
            std::cout << msg;
    }

    inline void message(const StringBlock& msg)
    {
        if (!flag_quiet)
            std::cout << msg;
    }

} // wasmgen
