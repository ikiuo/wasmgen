/*
 * utility.hpp
 */
#pragma once
#include "types.hpp"
#include "bitscan.hpp"

#define NOT_IMPLEMENTED_YET  assert(!"not implemented yet.")

namespace wasmgen
{
    void UNUSED(...) noexcept;

    template <typename T> bool in_range(T minval, T value, T maxval) noexcept;
    template <typename T> T clamp(T minval, T value, T maxval) noexcept;

    template <typename T> StdSet<T> operator + (const StdSet<T>& lhs, const StdSet<T>& rhs);
    template <typename T> StdSet<T> operator - (const StdSet<T>& lhs, const StdSet<T>& rhs);

    ////////////////
    // AutoRewind //
    ////////////////

    template <typename T>
    class AutoRewind
    {
    protected:
        T& data;
        T value;

    public:
        AutoRewind(T& data) noexcept;
        AutoRewind(T& data, T value) noexcept;
        ~AutoRewind() noexcept;
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    inline void UNUSED(...) noexcept {}

    template <typename T>
    inline bool in_range(T minval, T value, T maxval) noexcept
    {
        assert(minval <= maxval);
        return minval <= value && value <= maxval;
    }

    template <typename T>
    inline T clamp(T minval, T value, T maxval) noexcept
    {
        return std::max<T>(minval, std::min<T>(value, maxval));
    }

    /*
     * set<T> = set<T> + set<T>
     * set<T> = set<T> - set<T>
     */

    template <typename T>
    inline StdSet<T> operator + (const StdSet<T>& lhs, const StdSet<T>& rhs)
    {
        StdSet<T> rs = lhs;

        rs.insert(rhs.begin(), rhs.end());
        return rs;
    }

    template <typename T>
    inline StdSet<T> operator - (const StdSet<T>& lhs, const StdSet<T>& rhs)
    {
        StdSet<T> rs;

        for (const auto d : lhs)
            if (rhs.find(d) == rhs.end())
                rs.insert(d);
        return rs;
    }

    ////////////////
    // AutoRewind //
    ////////////////

    template <typename T>
    inline AutoRewind<T>::AutoRewind(T& data) noexcept
        : data(data)
        , value(data)
    {
        /*NOOP*/
    }

    template <typename T>
    inline AutoRewind<T>::AutoRewind(T& data, T newval) noexcept
        : data(data)
        , value(data)
    {
        data = newval;
    }

    template <typename T>
    inline AutoRewind<T>::~AutoRewind() noexcept
    {
        data = value;
    }

} // wasmgen
