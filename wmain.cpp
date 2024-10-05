/*
 * wmain.cpp
 */
#include "wmain.hpp"

#if USE_WMAIN
#include <Windows.h>
#include "utfcvt.hpp"

struct strlistinfo
{
    size_t size;
    size_t count;
    wchar_t** pp;
};

int main(int, char**, char**);

static strlistinfo analyze_strlist(wchar_t** pp)
{
    using namespace utfcvt;

    strlistinfo r { sizeof(char*) + 1, 0, pp };
    auto ss = pp;

    for (auto p = *pp; (p); p = * ++pp)
    {
        r.size += utf::to_utf8len(p).write;
        r.size += 1; // null:'\0'
    }
    r.count = pp - ss;
    r.size += r.count * sizeof(char*);
    return r;
}

static char** build_strlist(const strlistinfo& si, void* buffer)
{
    using namespace utfcvt;

    auto pp = si.pp;
    auto dd = (char**)buffer;
    auto d = (char*)(dd + si.count + 1);
    auto e = (char*)buffer + si.size - 1;

    for (auto p = *pp++; (p); p = *pp++)
    {
        auto buffer = utfcvt_buffer<char>(d, e);

        *dd++ = d;
        d += utf::cvt(buffer, p).write;
        *d++ = 0;
    }
    *dd = NULL;
    *d = 0;
    return (char**)buffer;
}

int wmain(int argc, wchar_t** wargv, wchar_t** wenvp)
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    auto ai = analyze_strlist(wargv);
    auto ei = analyze_strlist(wenvp);

    char** argv = build_strlist(ai, _alloca(ai.size));
    char** envp = build_strlist(ei, _alloca(ei.size));

    return main(argc, argv, envp);
}

#endif /* USE_WMAIN */
