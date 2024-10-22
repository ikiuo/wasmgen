/*
 * stdvec.hpp
 */
#pragma once
#include "compiler.hpp"

namespace wasmgen
{
    ///////////////
    // StdVector //
    ///////////////

    template <typename T>
    using StdVector = std::vector<T>;

    /////////////////
    // StdVecStack //
    /////////////////

    template <typename T>
    class StdVecStack : public StdVector<T>
    {
    private:
        using super = StdVector<T>;

    public:
        using super::super;

        operator bool() const noexcept;

        T pop();
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    /////////////////
    // StdVecStack //
    /////////////////

    template <typename T>
    inline StdVecStack<T>::operator bool() const noexcept
    {
        return !super::empty();
    }

    template <typename T>
    inline T StdVecStack<T>::pop()
    {
        T r = super::back();

        super::pop_back();
        return r;
    }

} // wasmgen
