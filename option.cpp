/*
 * option.cpp
 */
#include "option.hpp"

namespace wasmgen
{
    /**/

    const char* program = NULL;
    int flag_debug = 0;
    bool flag_quiet = false;
    bool flag_verbose = false;

    /**/

    StdString hexstr(const void* p, size_t n)
    {
        const std::uint8_t* q = (const std::uint8_t*)p;
        const std::uint8_t* e = q + n;
        StdString s;
        bool sep = false;

        while (q < e)
        {
            std::uint8_t d = *q++;

            if (sep)
                s += ' ';
            s += hexchr(d >> 4);
            s += hexchr(d >> 0);
            sep = true;
        }
        return s;
    }

    /**/

    void dumpbin(const void* addr, size_t size, size_t indent)
    {
        using sz_range = inc_range<size_t>;

        static const char hex[] = "0123456789abcdef";

        const uint8_t* bp = (const uint8_t*)addr;
        StdString prefix;

        for (auto _ : sz_range(indent))
            (void)_, prefix += ' ';

        int asize = 0;
        for (size_t n = size - 1; n > 0; n >>= 4)
            ++asize;
        asize = std::max<int>(3, asize - 1);

        for (auto y : irange<size_t>(0, size, 16))
        {
            size_t xsize = std::min<size_t>(16, size - y);
            StdString line = prefix;

            for (auto b : irange<int>(asize * 4, -4, -4))
                line += hex[(y >> b) & 15];
            line += ':';

            for (auto x : sz_range(xsize))
            {
                uint8_t b = bp[x + y];

                line += ' ';
                line += hex[b >> 4];
                line += hex[b & 15];
            }
            for (auto _ : sz_range(xsize, 16))
                (void)_, line += "   ";

            line += "  ";

            for (auto x : sz_range(xsize))
            {
                char b = bp[x + y];

                line += in_range<char>(0x20, b, 0x7e) ? b : '.';
            }
            for (auto _ : sz_range(xsize, 16))
                (void)_, line += ' ';

            line += '\n';

            message(line);
        }
    }

} // wasmgen
