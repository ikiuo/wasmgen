/*
 * stdstr.hpp
 */
#pragma once
#include "stdtypes.hpp"

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
    class StdStringMap : public StdMap<StdString, T>
    {
    private:
        using super = StdMap<StdString, T>;

    public:
        using key_type = typename super::key_type;

    public:
        using super::super;

        bool has(const key_type& key) const;

        T& get(const key_type& key, T& defval = T());
        const T& get(const key_type& key, const T& defval = T()) const;
    };

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

    template <typename T>
    inline bool StdStringMap<T>::has(const key_type& key) const
    {
#if !WASMGEN_COMPILER_CXX20
        return super::find(key) != super::end();
#else /* C++20 */
        return contains(key);
#endif
    }

    template <typename T>
    inline T& StdStringMap<T>::get(const key_type& key, T& defval)
    {
        auto it = super::find(key);

        return it != super::end() ? it->second : defval;
    }

    template <typename T>
    inline const T& StdStringMap<T>::get(const key_type& key, const T& defval) const
    {
        auto it = super::find(key);

        return it != super::end() ? it->second : defval;
    }

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
