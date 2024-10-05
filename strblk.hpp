/*
 * string_block.hpp
 */
#pragma once
#include "compiler.hpp"

namespace wasmgen
{
    ///////////
    // SBInt //
    ///////////

    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT> >
    class SBInt : public std::basic_string<CharT, Traits, Allocator>
    {
    public:
        using char_type = CharT;
        using traits_type = Traits;
        using allocator_type = Allocator;
    private:
        using super = std::basic_string<char_type, traits_type, allocator_type>;

    protected:
        using osstrem_type = std::basic_ostringstream<char_type, traits_type>;

    public:
        template <typename T> SBInt(T value, int width);
        template <typename T> SBInt(T value, int width, int fill);
        template <typename T> SBInt(bool upper, bool prefix, int base, int fill, int width, T value);
    };

    using CSBInt = SBInt<char>;

    ///////////
    // SBOct //
    ///////////

    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT> >
    class SBOct : public SBInt<CharT, Traits, Allocator>
    {
    private:
        using super = SBInt<CharT, Traits, Allocator>;

    public:
        template <typename T> SBOct(T value);
        template <typename T> SBOct(T value, int width, int mode);
    };

    using CSBOct = SBOct<char>;

    ///////////
    // SBHex //
    ///////////

    template <typename CharT,
              typename Traits = std::char_traits<CharT>,
              typename Allocator = std::allocator<CharT> >
    class SBHex : public SBInt<CharT, Traits, Allocator>
    {
    private:
        using super = SBInt<CharT, Traits, Allocator>;

    public:
        template <typename T> SBHex(T value);
        template <typename T> SBHex(T value, int width, int mode);

    protected:
        inline static bool is_X(int mode) { return mode == int('X'); };
        inline static bool is_Xx(int mode) { return is_X(mode) || mode == int('x'); };
        inline static bool is_Xx0(int mode) { return is_Xx(mode) || mode == int('0'); };
    };

    using CSBHex = SBHex<char>;

    /////////////////////
    // BaseStringBlock //
    /////////////////////

    template <typename OStringStream, typename Vector = std::vector<size_t> >
    class BaseStringBlock
    {
    public:
        using ostringstream_type = OStringStream;
        using vector_type = Vector;

        using char_type = typename ostringstream_type::char_type;
        using traits_type = typename ostringstream_type::traits_type;
        using string_allocator_type = typename ostringstream_type::allocator_type;

        using string_type = std::basic_string<char_type, traits_type, string_allocator_type>;

        using index_type = typename vector_type::value_type;
        using vector_allocator_type = typename vector_type::allocator_type;

        struct order : vector_type { using vector_type::vector_type; };

    protected:
        using ostream_type = std::basic_ostream<char_type, traits_type>;
        using initializer_list_type = std::initializer_list<BaseStringBlock>;

        using iosflag_function = std::ios_base& (*)(std::ios_base&);
        using ostream_function = ostream_type& (*)(ostream_type&);

        using setiosflags_type = decltype(std::setiosflags(std::ios_base::fmtflags(0)));
        using resetiosflags_type = decltype(std::resetiosflags(std::ios_base::fmtflags(0)));

        using setw_type = decltype(std::setw(0));
        using setprecision_type = decltype(std::setprecision(0));
        using setbase_type = decltype(std::setbase(10));

        using op_lshift = bool(*)(const BaseStringBlock*, ostream_type&);
        using destructor = void (*)(void*);

        /**/

        enum data_type {
            trivial_data,
            object_data,
            index_data,
            string_data,
        };

        /**/

        data_type mode;
        op_lshift lshift;
        destructor finish;

        inline static constexpr int size_max(size_t z)
            { return int(z); }
        inline static constexpr int size_max(size_t z, size_t y)
            { return int(z > y ? z : y); }
        template <typename... Args>
        inline static constexpr int size_max(size_t z, Args... args)
            { return int(z) > size_max(args...) ? int(z) : size_max(args...); }
        enum {
            buffer_size = size_max(
                sizeof(void*),
                sizeof(int),
                sizeof(long long),
                sizeof(double),
                sizeof(iosflag_function),
                sizeof(ostream_function),
                sizeof(string_type),
                sizeof(vector_type),
                sizeof(setiosflags_type),
                sizeof(resetiosflags_type),
                sizeof(setw_type),
                sizeof(setprecision_type),
                sizeof(setbase_type),
                0),

            buffer_align = size_max(
                alignof(void*),
                alignof(int),
                alignof(long long),
                alignof(double),
                alignof(iosflag_function),
                alignof(ostream_function),
                alignof(string_type),
                alignof(vector_type),
                alignof(setiosflags_type),
                alignof(resetiosflags_type),
                alignof(setw_type),
                alignof(setprecision_type),
                alignof(setbase_type),
                0),
        };
        alignas(buffer_align) std::uint8_t buffer[buffer_size];

        template <typename T>
        using is_small_object = typename std::conditional<
            (sizeof(T) <= buffer_size && alignof(T) <= buffer_align),
            std::true_type, std::false_type>::type;

        /**/

        struct trivial_tag { inline constexpr trivial_tag() noexcept {}; };
        struct small_object_tag { inline constexpr small_object_tag() {}; };
        struct large_object_tag { inline constexpr large_object_tag() {}; };

        template <typename T>
        using select_object_tag = typename std::conditional<
            is_small_object<T>::value, small_object_tag, large_object_tag>::type;
        template <typename T>
        using select_trivial_tag = typename std::conditional<
            is_small_object<T>::value && std::is_trivial<T>::value,
            trivial_tag, select_object_tag<T>>::type;
        template <typename T>
        struct select_function_tag
        {
            template <bool B> struct funcref { using type = select_trivial_tag<T>; };
            template <> struct funcref<true> { using type = trivial_tag; };
            using type = typename funcref<std::is_function<T>::value>::type;
        };
        template <typename T>
        using select_tag = typename select_function_tag<T>::type;

        /**/

        static bool lshift_none(const BaseStringBlock* s, ostream_type& o)
            { (void)s; (void)o; return false; }

        template <typename T>
        static bool lshift_data(const BaseStringBlock* s, ostream_type& o)
            { o << *(T*)s->buffer; return true; }

        template <typename T>
        static bool lshift_object(const BaseStringBlock* s, ostream_type& o)
            { o << **(T**)s->buffer; return true; }

        inline bool write(ostringstream_type& o) const
            { return lshift(this, o); }

        template <typename T> static void discard(void* p) noexcept { ((T*)p)->~T(); }
        template <typename T> static void destroy(void* p) noexcept { delete *(T**)p; }

        inline string_type& strbuf() noexcept { return *(string_type*)buffer; }
        inline const string_type& strbuf() const noexcept { return *(string_type*)buffer; }

        inline vector_type& vecbuf() noexcept { return *(vector_type*)buffer; }
        inline const vector_type& vecbuf() const noexcept { return *(vector_type*)buffer; }

    public:

        inline const string_type& str() const noexcept
            {
                static string_type s;
                return mode == string_data ? strbuf() : s;
            }
        inline const char_type* c_str() const noexcept { return str().c_str(); }
        inline operator const string_type&() const noexcept { return str(); }
        inline operator const char_type*() const noexcept { return c_str(); }

        /**/

        inline BaseStringBlock(ostream_function n) noexcept
            : BaseStringBlock(n, trivial_tag()) {}

        inline BaseStringBlock(const char_type* s)
            : mode(string_data)
            , lshift(lshift_data<string_type>)
            , finish(discard<string_type>)
            { new (buffer) string_type(s); }

        inline BaseStringBlock(const string_type& s)
            : BaseStringBlock(s, string_data, small_object_tag()) {}

        inline BaseStringBlock(const order& data)
            : mode(index_data), lshift(lshift_none), finish(discard<order>)
            { new (buffer) order(data); }

        template <typename T>
        inline BaseStringBlock(const T& s) noexcept(noexcept(select_tag<T>()))
            : BaseStringBlock(s, select_tag<T>()) {}

    protected:

        template <typename T>
        inline BaseStringBlock(T n, trivial_tag) noexcept
            : mode(trivial_data), lshift(lshift_data<T>), finish()
            { new (buffer) T(n); }

        template <typename T>
        inline BaseStringBlock(const T& s, small_object_tag)
            : BaseStringBlock(s, object_data, small_object_tag()) {}

        template <typename T>
        inline BaseStringBlock(const T& s, data_type t, small_object_tag)
            : mode(t), lshift(lshift_data<T>), finish(discard<T>)
            { new (buffer) T(s); }

        template <typename T>
        inline BaseStringBlock(const T& s, large_object_tag)
            : mode(object_data), lshift(lshift_object<T>), finish(destroy<T>)
            { *(T**)buffer = new T(s); }

    public:

        BaseStringBlock(const initializer_list_type& args);
        inline ~BaseStringBlock() { if (finish) finish(buffer); }

#if WASMGEN_COMPILER_CXX17
    protected:
#endif /* C++17 */

        const BaseStringBlock& operator =(const BaseStringBlock& s) = delete;

        inline BaseStringBlock(const BaseStringBlock& s) noexcept // = delete
            : BaseStringBlock(s.str()) {}
    };

    /////////////////
    // StringBlock //
    /////////////////

    using StringBlock = BaseStringBlock<std::ostringstream>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ///////////
    // SBInt //
    ///////////

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBInt<CharT, Traits, Allocator>::SBInt(T value, int width)
        : SBInt(false, false, 10, 0, width, value) {}

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBInt<CharT, Traits, Allocator>::SBInt(T value, int width, int fill)
        : SBInt(false, false, 10, fill, width, value) {}

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBInt<CharT, Traits, Allocator>::SBInt(
        bool upper, bool prefix, int base, int fill, int width, T value)
    {
        osstrem_type o;

        switch (base)
        {
        case +8:
            o << std::oct;
            break;

        case 16:
            if (prefix)
                o << (upper ? "0X" : "0x");
            if (upper)
                o << std::uppercase;
            o << std::hex;
            break;

        default:
            break;
        }
        if (fill)
            o << std::setfill(char_type(fill));
        if (width > 0)
            o << std::setw(width);
        o << value;

        static_cast<super&>(*this) = std::move(o.str());
    }

    ///////////
    // SBOct //
    ///////////

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBOct<CharT, Traits, Allocator>::SBOct(T value)
        : super(false, false, 8, 0, 0, value) {}

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBOct<CharT, Traits, Allocator>::SBOct(T value, int width, int mode)
        : super(false, false, 8,
                mode == int('O') || mode == int('o') || mode == int('0') ? '0' : 0,
                width, value) {}

    ///////////
    // SBHex //
    ///////////

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBHex<CharT, Traits, Allocator>::SBHex(T value)
        : super(false, false, 16, 0, 0, value) {}

    template <typename CharT, typename Traits, typename Allocator>
    template <typename T>
    inline SBHex<CharT, Traits, Allocator>::SBHex(T value, int width, int mode)
        : super(is_X(mode), is_Xx(mode), 16, is_Xx0(mode) ? '0' : 0, width, value) {}

    /////////////////////
    // BaseStringBlock //
    /////////////////////

    template <typename OStringStream, typename Vector>
    BaseStringBlock<OStringStream, Vector>::BaseStringBlock(const initializer_list_type& args)
        : mode(string_data)
        , lshift(lshift_data<string_type>)
        , finish(discard<string_type>)
    {
        using iterator = typename initializer_list_type::const_iterator;

        if (!args.size())
        {
            new (buffer) string_type;
            return;
        }

        ostringstream_type stream;
        iterator top = args.begin();

        if (top->mode != index_data)
            for (auto& child : args)
                child.write(stream);
        else
            for (auto n : top->vecbuf())
                if (0 < n && size_t(n) < args.size())
                    (top + n)->write(stream);

        new (buffer) string_type(std::move(stream.str()));
    }

} // wasmgen
