/*
 * string.cpp
 */
#include "string.hpp"

namespace wasmgen
{
    ////////////
    // String //
    ////////////

    String::String()
    {
        /*NOOP*/
    }

    String::~String()
    {
        /*NOOP*/
    }

    String* String::replace(int s, int t) const
    {
        NewString r;

        for (int c : *this)
            r->push_back(c != s ? c : t);
        return Finish(r);
    }

    StringList* String::split(int c, int maxsplit) const
    {
        NewStringList r;
        auto pit = begin();
        auto eit = end();
        auto nit = pit;
        int count = 0;

        while (nit != eit)
        {
            if (*nit != c)
            {
                ++nit;
                continue;
            }

            r->push_back(new String(pit, nit));
            pit = ++nit;

            if (++count == maxsplit)
                break;
        }
        if (pit != eit)
            r->push_back(new String(pit, eit));
        return Finish(r);
    }

    /////////////////////
    // SingletonString //
    /////////////////////

    SingletonString::SingletonString(const char* s)
        : super(new String(s))
    {
        incref();
    }

} // wasmgen
