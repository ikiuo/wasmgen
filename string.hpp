/*
 * string.hpp
 */
#pragma once
#include "array.hpp"

/*
 * utfcvt
 */
#if __cpp_char8_t < 201811L
#define utfcvt_compiler_char8     1
#define utfcvt_compiler_u8string  1

using char8_t = char;

namespace std
{
    using u8string = basic_string<char8_t>;
}

#endif /* __cpp_char8_t */
#include "utfcvt.hpp"

namespace wasmgen
{
    class String;
    using StringList = PointerArray<String>;

    ////////////
    // String //
    ////////////

    class String
        : virtual public Object
        , public StdString
    {
    public:
        using string_type = StdString;
        using string_type::iterator;
        using string_type::const_iterator;
        using string_type::value_type;
        using char_type = value_type;

    public:
        String();
        String(const StdString& s);

        template <typename... Args>
        String(Args... args);

        ~String();

        operator const char*() const noexcept;

        String* set(const char* text);

        String* replace(int s, int t) const;
        StringList* split(int c, int maxsplit = 0) const;
        String* join(const StringList* list) const;
        template <typename T> String* join(T begin, T end) const;
    };

    using CharType = String::char_type;
    using UCharType = std::make_unsigned<CharType>::type;

    using StringPtr = Pointer<String>;
    using StringRef = RefPointer<String>;
    using NewString = NewPointer<String>;

    ////////////////
    // StringList //
    ////////////////

    using StringListPtr = Pointer<StringList>;
    using StringListRef = RefPointer<StringList>;
    using NewStringList = NewPointer<StringList>;

    /////////////////////
    // SingletonString //
    /////////////////////

    class SingletonString : public Singleton<String>
    {
    private:
        using super = Singleton<String>;

    public:
        SingletonString(const char* s);
    };

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////
    // String //
    ////////////

    inline String::String(const StdString& s)
        : Object()
        , string_type(s)
    {
        /*NOOP*/
    }

    template <typename... Args>
    String::String(Args... args)
        : Object()
        , string_type(args...)
    {
        /*NOOP*/
    }

    /**/

    inline String::operator const char*() const noexcept
    {
        return c_str();
    }

    inline String* String::set(const char* text)
    {
        *static_cast<StdString*>(this) = text;
        return this;
    }

    inline String* String::join(const StringList* list) const
    {
        return join(list->begin(), list->end());
    }

    template <typename T>
    String* String::join(T begin, T end) const
    {
        NewString r;
        const String* s = nullptr;

        for (T it = begin; it != end; ++it)
        {
            const String* p = *it;

            if (s)
                *r += *s;
            if (p)
                *r += *p;
            s = this;
        }
        return Finish(r);
    }

} // wasmgen
