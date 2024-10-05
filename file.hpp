/*
 * file.hpp
 */
#pragma once
#include "array.hpp"
#include "string.hpp"

namespace wasmgen
{
    using FileGetCRes = utfcvt::utfcvt_getcres;

    extern NewStringList includes;

    /////////////////
    // FileTextPos //
    /////////////////

    struct FileTextPos
    {
        size_t line;
        size_t column;

        FileTextPos() noexcept = default;
        constexpr FileTextPos(size_t line, size_t column) noexcept;

        constexpr void clear() noexcept;
        constexpr void set(size_t line, size_t column) noexcept;
    };

    ////////////////
    // FileString //
    ////////////////

    class FileString : public String
    {
    private:
        using super = String;

    public:
        StringPtr file_name;
        FileTextPos text_pos;

    public:
        FileString();
        FileString(FileString* s);
        FileString(FileString& s);

        template <typename... Args>
        FileString(Args... args);

        static SingletonString null;
        static SingletonString empty;

        static SingletonString utf8;
        static SingletonString utf8_bom;
        static SingletonString utf16be;
        static SingletonString utf16le;
        static SingletonString utf32be;
        static SingletonString utf32le;

        static SingletonString dir_dot;
        static SingletonString dir_sep;

        static String* safe_name(String* name);


        static bool isdirsep(int c) noexcept;
        static bool isdrive(String* path) noexcept;
        static bool isdriveroot(String* path) noexcept;
        static bool isuncpath(String* path) noexcept;
        static bool isrootdir(String* path) noexcept;

        static StringList* pathdirfile(String* path);

        static String* getcurdir();
        static String* updatedir(String* curdir, String* file);
        static StringList* dirandfile(String* curdir, String* file);
    };

    using FileStringPtr = Pointer<FileString>;
    using FileStringRef = RefPointer<FileString>;
    using NewFileString = NewPointer<FileString>;

    //////////////
    // FileData //
    //////////////

    class FileData : virtual public Object
    {
    protected:
        const void *data_addr;
        size_t data_size;

    public:
        FileData() noexcept;
        FileData(const void* data, size_t size) noexcept;

        const void* address() noexcept;
        size_t size() noexcept;
    };

    using FileDataPtr = Pointer<FileData>;
    using FileDataRef = RefPointer<FileData>;
    using NewFileData = NewPointer<FileData>;

    ////////////////////
    // FileStreamData //
    ////////////////////

    class FileStreamData : public FileData
    {
    protected:
        StdString data;

    public:
        FileStreamData(std::FILE* fp);
        FileStreamData(const StdString& text);
    };

    ////////////////
    // FileReader //
    ////////////////

    class FileReader : virtual public Object
    {
    protected:
        StringPtr name;
        StringPtr encode;
        FileDataPtr data;
        bool loaded;

    public:
        FileReader(const char* path);
        FileReader(String* file);
        FileReader(FileData* data, String* name = nullptr);

        bool valid() noexcept;

        const char* c_path() noexcept;
        String* path() noexcept;

        const void* address() noexcept;
        size_t size() noexcept;
    };

    using FileReaderPtr = Pointer<FileReader>;
    using FileReaderRef = RefPointer<FileReader>;
    using NewFileReader = NewPointer<FileReader>;

    ////////////////////
    // TextFileReader //
    ////////////////////

    class TextFileReader : public FileReader
    {
    private:
        using super = FileReader;

    protected:
        size_t text_size;
        size_t text_remain;
        const uint8_t* text_top;
        const uint8_t* text_end;
        const void* text_pos;

        uint32_t replace;
        utfcvt::utfcvt_nextcode getcode;
        int line;

    public:
        TextFileReader(const char* file);
        TextFileReader(const StdString& file);
        TextFileReader(String* file);
        TextFileReader(FileData* data, String* name = nullptr);

        void initialize();

        const char* c_encoding() noexcept;
        String* encoding() noexcept;

        size_t remain() noexcept;
        FileGetCRes getchar() noexcept;

        FileString* readline();
        template <typename T>
        bool readline(T& d);

        std::u32string full();

        void show_info();
    };

    using TextFileReaderPtr = Pointer<TextFileReader>;
    using TextFileReaderRef = RefPointer<TextFileReader>;
    using NewTextFileReader = NewPointer<TextFileReader>;

    ////////////////////////
    // TextFileReaderList //
    ////////////////////////

    using TextFileReaderList = PointerArray<TextFileReader>;

    using TextFileReaderListPtr = Pointer<TextFileReaderList>;
    using TextFileReaderListRef = RefPointer<TextFileReaderList>;
    using NewTextFileReaderList = NewPointer<TextFileReaderList>;

    ////////////////
    // FileWriter //
    ////////////////

    class FileWriter : virtual public Object
    {
    protected:
        StringPtr name;
        FILE* handle;

    public:
        FileWriter(const char* file);
        FileWriter(const StdString &file);
        FileWriter(String* file);
        ~FileWriter();

        bool valid() noexcept;
        void close() noexcept;

        bool write(const ByteArray& data) noexcept;
    };

    using FileWriterPtr = Pointer<FileWriter>;
    using FileWriterRef = RefPointer<FileWriter>;
    using NewFileWriter = NewPointer<FileWriter>;

} // wasmgen

/*
 *
 */

namespace wasmgen
{
    ////////////////
    // FileString //
    ////////////////

    inline bool FileString::isdirsep(int c) noexcept
    {
        return c == '/' || (WASMGEN_TARGET_WINDOWS && c == '\\');
    }

    inline bool FileString::isdrive(String* path) noexcept
    {
#if WASMGEN_TARGET_WINDOWS
        return path && path->size() >= 2 &&
            isalpha((*path)[0]) && (*path)[1] == ':';
#else
        UNUSED(path);
        return false;
#endif
    }

    inline bool FileString::isdriveroot(String* path) noexcept
    {
#if WASMGEN_TARGET_WINDOWS
        return isdrive(path) && path->size() >= 3 && isdirsep((*path)[2]);
#else
        UNUSED(path);
        return false;
#endif
    }

    inline bool FileString::isuncpath(String* path) noexcept
    {
#if WASMGEN_TARGET_WINDOWS
        return path && path->size() >= 2 &&
            (*path)[0] == '\\' && (*path)[1] == '\\';
#else
        UNUSED(path);
        return false;
#endif
    }

    inline bool FileString::isrootdir(String* path) noexcept
    {
        return path && (isuncpath(path) || isdriveroot(path) ||
                        (path->size() && isdirsep((*path)[0])));
    }

    /////////////////
    // FileTextPos //
    ////////////////

    inline constexpr FileTextPos::FileTextPos(size_t l, size_t c) noexcept
        : line(l)
        , column(c)
    {
        /*NOOP*/
    }

    /**/

    inline constexpr void FileTextPos::clear() noexcept
    {
        set(0, 0);
    }

    inline constexpr void FileTextPos::set(size_t l, size_t c) noexcept
    {
        line = l;
        column = c;
    }

    ////////////////
    // FileString //
    ////////////////

    inline FileString::FileString(FileString& s) : FileString(&s) {}

    template <typename... Args>
    FileString::FileString(Args... args)
        : super(args...)
        , file_name(null)
        , text_pos(0, 0)
    {
        /*NOOP*/
    }

    /**/

    inline String* FileString::safe_name(String* name)
    {
        return name ? name : null;
    }

    //////////////
    // FileData //
    //////////////

    inline FileData::FileData() noexcept : FileData(nullptr, 0) {}

    inline const void* FileData::address() noexcept
    {
        return data_addr;
    }

    inline size_t FileData::size() noexcept
    {
        return data_size;
    }

    ////////////////
    // FileReader //
    ////////////////

    inline FileReader::FileReader(const char* path)
        : FileReader(&NewString(path)) {}

    /**/

    inline bool FileReader::valid() noexcept
    {
        return loaded;
    }

    inline const char* FileReader::c_path() noexcept
    {
        return name->c_str();
    }

    inline String* FileReader::path() noexcept
    {
        return name;
    }

    inline const void* FileReader::address() noexcept
    {
        return data->address();
    }

    inline size_t FileReader::size() noexcept
    {
        return data->size();
    }

    ////////////////////
    // TextFileReader //
    ////////////////////

    inline TextFileReader::TextFileReader(const char* file)
        : TextFileReader(&NewString(file)) {}

    inline TextFileReader::TextFileReader(const StdString& file)
        : TextFileReader(&NewString(file)) {}

    /**/

    inline const char* TextFileReader::c_encoding() noexcept
    {
        return encode->c_str();
    }

    inline String* TextFileReader::encoding() noexcept
    {
        return encode;
    }

    inline FileString* TextFileReader::readline()
    {
        NewFileString text(new FileString);

        if (!text)
            return nullptr;

        text->file_name = name;
        text->text_pos.line = ++line;
        return readline(*text) ? Finish(text) : nullptr;
    }

    template <typename T>
    inline bool TextFileReader::readline(T& d)
    {
        d.clear();
        if (!remain())
            return false;
        while (remain())
        {
            FileGetCRes res = getchar();
            utfcvt::utf32_t c = res.success ? res.code : replace;

            utfcvt::utf::putcode(d, c);
            if (c == '\n')
                break;
        }
        return true;
    }

    inline size_t TextFileReader::remain() noexcept
    {
        return text_end - (const uint8_t*)text_pos;
    }

    inline FileGetCRes TextFileReader::getchar() noexcept
    {
        return getcode(text_pos, text_end);
    }

    ////////////////
    // FileWriter //
    ////////////////

    inline FileWriter::FileWriter(const char* file)
        : FileWriter(&NewString(file)) {}

    inline FileWriter::FileWriter(const StdString &file)
        : FileWriter(&NewString(file)) {}

    inline bool FileWriter::valid() noexcept
    {
        return handle;
    }

    inline void FileWriter::close() noexcept
    {
        if (handle)
        {
            ::fclose(handle);
            handle = nullptr;
        }
    }

} // wasmgen
