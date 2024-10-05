/*
 * object.cpp
 */
#include "object.hpp"

namespace wasmgen
{
    ////////////
    // Object //
    ////////////

    Object::Object() noexcept
        : reference_count(0)
    {
        /*NOOP*/
    }

} // wasmgen
