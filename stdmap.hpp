/*
 * stdmap.hpp
 */
#pragma once
#include "stdtypes.hpp"

namespace wasmgen
{
    ////////////
    // StdMap //
    ////////////

    template <typename K, typename T>
    class StdMap : public std::map<K, T>
    {
    private:
        using super = std::map<K, T>;

    public:
        using key_type = typename super::key_type;
        using mapped_type = typename super::mapped_type;
        using value_type = typename super::value_type;

    public:
        using super::super;

        bool has(const key_type& key) const noexcept;

        mapped_type& get(const key_type& key, mapped_type& defval = mapped_type());
        const mapped_type& get(const key_type& key, const mapped_type& defval = mapped_type()) const;
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // StdMap //
    ////////////

    template <typename K, typename T>
    inline bool StdMap<K,T>::has(const key_type& key) const noexcept
    {
#if !WASMGEN_COMPILER_CXX20
        return super::find(key) != super::end();
#else /* C++20 */
        return super::contains(key);
#endif
    }

    template <typename K, typename T>
    inline typename StdMap<K,T>::mapped_type&
    StdMap<K,T>::get(const key_type& key, mapped_type& defval)
    {
        auto it = super::find(key);

        return it != super::end() ? it->second : defval;
    }

    template <typename K, typename T>
    inline const typename StdMap<K,T>::mapped_type&
    StdMap<K,T>::get(const key_type& key, const mapped_type& defval) const
    {
        auto it = super::find(key);

        return it != super::end() ? it->second : defval;
    }

} // wasmgen
