/*
 * array.cpp
 */
#include "array.hpp"

namespace wasmgen
{
    ///////////////
    // ByteArray //
    ///////////////

    const char ByteArray::base64_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/=";

    ByteArray ByteArray::base64() const
    {
        size_t ssz = size();
        ByteArray b64(((ssz + 2) / 3) << 2);
        const uint8_t* src = data();
        uint8_t *dst = b64.data();
        const char* tab = base64_table;
        size_t pos = 0;

        for (pos = 0; (pos + 3) <= ssz; pos += 3)
        {
            uint32_t data = 0;

            data |= src[pos + 0] << 16;
            data |= src[pos + 1] <<  8;
            data |= src[pos + 2] <<  0;

            dst[0] = tab[(data >> 18) & 0x3f];
            dst[1] = tab[(data >> 12) & 0x3f];
            dst[2] = tab[(data >>  6) & 0x3f];
            dst[3] = tab[(data >>  0) & 0x3f];
            dst += 4;
        }
        if (pos < ssz)
        {
            uint32_t data = 0;

            data |= src[pos] << 16;

            bool e2 = ++pos < ssz;

            if (e2)
                data |= src[pos] <<  8;

            dst[0] = tab[(data >> 18) & 0x3f];
            dst[1] = tab[(data >> 12) & 0x3f];
            dst[2] = tab[(data >>  6) & 0x3f];
            if (!e2)
                dst[2] = '=';
            dst[3] = '=';
        }
        return b64;
    }

} // wasmgen
