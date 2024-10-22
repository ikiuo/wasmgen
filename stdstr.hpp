/*
 * stdstr.hpp
 */
#pragma once
#include "stdtypes.hpp"
#include "stdmap.hpp"

namespace wasmgen
{
    /////////////////
    // StdCStrList //
    /////////////////

    using StdCStrList = StdVector<const char*>;

    ///////////////////
    // StdStringList //
    ///////////////////

    using StdStringList = StdVector<StdString>;

    //////////////////
    // StdStringSet //
    //////////////////

    class StdStringSet : public StdSet<StdString>
    {
    private:
        using super = StdSet<StdString>;

    public:
        using super::super;

        bool has(const key_type& key) const;
    };

    //////////////////
    // StdStringMap //
    //////////////////

    template <typename T>
    using StdStringMap = StdMap<StdString, T>;

    /**/

    template <typename T>
    StdStringMap<T> operator | (const StdStringMap<T>& lhs, const StdStringMap<T>& rhs);

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    //////////////////
    // StdStringSet //
    //////////////////

    inline bool StdStringSet::has(const key_type& key) const
    {
#if !WASMGEN_COMPILER_CXX20
        return find(key) != end();
#else /* C++20 */
        return contains(key);
#endif
    }

    //////////////////
    // StdStringMap //
    //////////////////

    /*
     * StdStringMap<T> = StdStringMap<T> | StdStringMap<T>
     */

    template <typename T>
    inline StdStringMap<T> operator | (const StdStringMap<T>& lhs, const StdStringMap<T>& rhs)
    {
        StdStringMap<T> rd = lhs;

        for (auto& kv : rhs)
            rd[kv.first] = kv.second;
        return rd;
    }

} // wasmgen
