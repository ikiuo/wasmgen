/*
 * stdtypes.hpp
 */
#pragma once
#include "compiler.hpp"

namespace wasmgen
{
    template <typename K, typename V>
    using StdMap = std::map<K, V>;

    template <typename T>
    using StdSet = std::set<T>;

    using StdString = std::string;

    template <typename T>
    using StdVector = std::vector<T>;

} // wasmgen
