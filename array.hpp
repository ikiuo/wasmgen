/*
 * array.hpp
 */
#pragma once
#include "object.hpp"

namespace wasmgen
{
    ///////////////
    // ByteArray //
    ///////////////

    class ByteArray : public StdVector<uint8_t>
    {
    private:
        using super = StdVector<uint8_t>;

    protected:
        static const char base64_table[];

    public:
        using super::super;

        void append_bytes(const void* s, size_t n);
        template <typename T> void append_object(const T& value);

        template <typename T> void append_leb128(const T& value);

        void append_binary(const ByteArray& value);
        void append_vector(const ByteArray& value);

        void append_name(const StdString& name);

        ByteArray base64() const;
    };

    ///////////
    // Array //
    ///////////

    template <typename T>
    class Array
        : virtual public Object
        , public StdVector<T>
    {
    public:
        using vector = StdVector<T>;

    public:
        Array() = default;
        template <typename... Args> Array(Args... args);
    };

    template <typename T> using ArrayPtr = Pointer<Array<T>>;
    template <typename T> using ArrayRef = RefPointer<Array<T>>;
    template <typename T> using NewArray = NewPointer<Array<T>>;

    /////////////
    // ArrayS8 //
    /////////////

    using ArrayS8 = Array<int8_t>;
    using ArrayS8Ptr = ArrayPtr<int8_t>;
    using ArrayS8Ref = ArrayRef<int8_t>;
    using NewArrayS8 = NewArray<int8_t>;

    //////////////
    // ArrayS16 //
    //////////////

    using ArrayS16 = Array<int16_t>;
    using ArrayS16Ptr = ArrayPtr<int16_t>;
    using ArrayS16Ref = ArrayRef<int16_t>;
    using NewArrayS16 = NewArray<int16_t>;

    //////////////
    // ArrayS32 //
    //////////////

    using ArrayS32 = Array<int32_t>;
    using ArrayS32Ptr = ArrayPtr<int32_t>;
    using ArrayS32Ref = ArrayRef<int32_t>;
    using NewArrayS32 = NewArray<int32_t>;

    //////////////
    // ArrayS64 //
    //////////////

    using ArrayS64 = Array<int64_t>;
    using ArrayS64Ptr = ArrayPtr<int64_t>;
    using ArrayS64Ref = ArrayRef<int64_t>;
    using NewArrayS64 = NewArray<int64_t>;

    /////////////
    // ArrayU8 //
    /////////////

    using ArrayU8 = Array<uint8_t>;
    using ArrayU8Ptr = ArrayPtr<uint8_t>;
    using ArrayU8Ref = ArrayRef<uint8_t>;
    using NewArrayU8 = NewArray<uint8_t>;

    //////////////
    // ArrayU16 //
    //////////////

    using ArrayU16 = Array<uint16_t>;
    using ArrayU16Ptr = ArrayPtr<uint16_t>;
    using ArrayU16Ref = ArrayRef<uint16_t>;
    using NewArrayU16 = NewArray<uint16_t>;

    //////////////
    // ArrayU32 //
    //////////////

    using ArrayU32 = Array<uint32_t>;
    using ArrayU32Ptr = ArrayPtr<uint32_t>;
    using ArrayU32Ref = ArrayRef<uint32_t>;
    using NewArrayU32 = NewArray<uint32_t>;

    //////////////
    // ArrayU64 //
    //////////////

    using ArrayU64 = Array<uint64_t>;
    using ArrayU64Ptr = ArrayPtr<uint64_t>;
    using ArrayU64Ref = ArrayRef<uint64_t>;
    using NewArrayU64 = NewArray<uint64_t>;

    //////////////////
    // PointerArray //
    //////////////////

    template <typename T>
    class PointerArray : public Array<Pointer<T>>
    {
    private:
        using super = Array<Pointer<T>>;

    public:
        using super::super;

        void push_back(T *object);
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ///////////////
    // ByteArray //
    ///////////////

    inline void ByteArray::append_bytes(const void* s, size_t n)
    {
        const uint8_t* p = (const uint8_t*) s;

        insert(end(), p, p + n);
    }

    template <typename T>
    inline void ByteArray::append_object(const T& value)
    {
        append_bytes(&value, sizeof(value));
    }

    template <typename T>
    inline void ByteArray::append_leb128(const T& value)
    {
        LEB128 enc(value);

        append_bytes(enc.data, enc.size);
    }

    inline void ByteArray::append_binary(const ByteArray& value)
    {
        insert(end(), value.begin(), value.end());
    }

    inline void ByteArray::append_vector(const ByteArray& value)
    {
        append_leb128(uint32_t(value.size()));
        append_binary(value);
    }

    inline void ByteArray::append_name(const StdString& name)
    {
        append_leb128(uint32_t(name.size()));
        insert(end(), name.begin(), name.end());
    }

    ///////////
    // Array //
    ///////////

    template <typename T>
    template <typename... Args>
    Array<T>::Array(Args... args)
        : vector(args...)
    {
        /*NOOP*/
    }

    //////////////////
    // PointerArray //
    //////////////////

    template <typename T>
    inline void PointerArray<T>::push_back(T* object)
    {
        super::push_back(object);
    }

} // wasmgen
