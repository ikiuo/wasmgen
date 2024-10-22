/*
 * stdstr.hpp
 */
#pragma once
#include "compiler.hpp"

namespace wasmgen
{
    ////////////
    // StdSet //
    ////////////

    template <typename T>
    class StdSet : public std::set<T>
    {
    private:
        using super = std::set<T>;

    public:
        using key_type = typename super::key_type;
        using value_type = typename super::value_type;

    public:
        using super::super;

        bool has(const key_type& key) const noexcept;
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // StdSet //
    ////////////

    template <typename T>
    inline bool StdSet<T>::has(const key_type& key) const noexcept
    {
#if !WASMGEN_COMPILER_CXX20
        return super::find(key) != super::end();
#else /* C++20 */
        return super::contains(key);
#endif
    }

} // wasmgen
