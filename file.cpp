/*
 * file.cpp
 */
#include "file.hpp"

#if WASMGEN_TARGET_UNIX
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#elif WASMGEN_TARGET_WINDOWS
#include <Windows.h>
#undef DECREF
#else /* WASMGEN_TARGET_??? */
#error "unknown target"
#endif

namespace wasmgen
{
    NewStringList includes;

    ////////////////
    // FileString //
    ////////////////

    SingletonString FileString::null("(null)");
    SingletonString FileString::empty("");

    SingletonString FileString::utf8("UTF-8");
    SingletonString FileString::utf8_bom("UTF-8[BOM]");
    SingletonString FileString::utf16be("UTF-16[BOM:BE]");
    SingletonString FileString::utf16le("UTF-16[BOM:LE]");
    SingletonString FileString::utf32be("UTF-32[BOM:BE]");
    SingletonString FileString::utf32le("UTF-32[BOM:LE]");

    SingletonString FileString::dir_dot(".");
    SingletonString FileString::dir_sep("/");

    /**/

    FileString::FileString()
        : super()
        , file_name()
        , text_pos(0, 0)
    {
        /*NOOP*/
    }

    FileString::FileString(FileString* s)
        : super(*s)
        , file_name(s->file_name)
        , text_pos(s->text_pos)
    {
        /*NOOP*/
    }

    /**/

    StringList* FileString::pathdirfile(String* path)
    {
        NewStringList r;
        auto bit = path->begin();
        auto eit = path->end();
        auto pit = bit;

        for (auto it = bit; it != eit; ++it)
            if (isdirsep(*it))
                pit = it;
        if (isdirsep(*pit))
        {
            auto sit = pit++;

            r->push_back(new String(bit, sit));
            r->push_back(new String(sit, pit));
            r->push_back(new String(pit, eit));
        }
        else
        {
            r->push_back(nullptr);
            r->push_back(nullptr);
            r->push_back(path);
        }
        return Finish(r);
    }

    String* FileString::getcurdir()
    {
#if WASMGEN_TARGET_WINDOWS
        char* buf = ::_getcwd(nullptr, 0);
#else /* if WASMGEN_TARGET_UNIX */
        char* buf = ::getcwd(nullptr, 0);
#endif
        if (!buf)
            throw OSError("getcwd が失敗しました。");

        NewString r(buf);

        ::free(buf);

        return Finish(r);
    }

    String* FileString::updatedir(String* curdir, String* file)
    {
        StringPtr r = curdir;

        assert(file);
        if (file->size())
        {
            StringListRef pnf = pathdirfile(file);
            auto& fdir((*pnf)[0]);

            if (fdir)
            {
                if (isrootdir(fdir) || *curdir == *dir_dot)
                    return Finish(fdir);
                r = new String(*curdir + (*pnf)[1] + (*pnf)[0]);
            }
        }
        return Finish(r);
    }

    StringList* FileString::dirandfile(String* curdir, String* file)
    {
        assert(curdir);
        assert(file);

        WASMGEN_DEBUG(2, "FileString::dirandfile(\"", curdir->c_str(), "\", \"", file->c_str(), "\"):\n");

        NewStringList r;
        StringListRef pnf;
        String* fdir = nullptr;
        String* fsep = nullptr;
        String* fname = file;

        if (file->size())
        {
            pnf = pathdirfile(file); assert(pnf->size() >= 3);
            fdir = (*pnf)[0];
            fsep = (*pnf)[1];
            fname = (*pnf)[2];
        }
        if (!fsep)
            fsep = FileString::dir_sep;
        if (fdir)
        {
            if (isrootdir(fdir))
            {
                r->push_back(Transfer(fdir));
                r->push_back(file);
            }
            else
            {
                NewString d(*curdir + *fsep + *fdir);
                NewString f(*d + *fsep + *fname);

                r->push_back(Transfer(d));
                r->push_back(Transfer(f));
            }
        }
        else
        {
            r->push_back(curdir);
            if (*curdir == *dir_dot)
                r->push_back(file);
            else
                r->push_back(new String(*curdir + *fsep + *fname));
        }

        WASMGEN_DEBUG(2, "   [0]: ", (*r)[0]->c_str(), "\n");
        WASMGEN_DEBUG(2, "   [1]: ", (*r)[1]->c_str(), "\n");
        return Finish(r);
    }

    //////////////
    // FileData //
    //////////////

    FileData::FileData(const void* data, size_t size) noexcept
        : data_addr(data)
        , data_size(size)
    {
        /*NOOP*/
    }

    ////////////////////
    // FileStreamData //
    ////////////////////

    FileStreamData::FileStreamData(FILE* fp)
    {
        size_t bufsz = 64 * 1024;
        ByteArray bufobj(bufsz);
        uint8_t *buffer = bufobj.data();

        for (;;)
        {
            size_t rsz = fread(buffer, 1, bufsz, fp);

            if (rsz > 0)
                data.insert(data.end(), buffer, buffer + rsz);
            if (rsz < bufsz)
                break;
        }

        data_size = data.size();
        data_addr = data.data();
    }

    FileStreamData::FileStreamData(const StdString& text)
        : data(text)
    {
        data_size = data.size();
        data_addr = data.data();
    }

    /////////////////
    // FileMapping //
    /////////////////

    class FileMapping : public FileData
    {
    protected:
        bool opened;
#if WASMGEN_TARGET_UNIX
        int fd;
#elif WASMGEN_TARGET_WINDOWS
        HANDLE m_hFile;
        HANDLE m_hMap;
#endif

    public:
        FileMapping();
        FileMapping(String* path);
        ~FileMapping();

        bool isopened() noexcept;

        void reset() noexcept;
        bool open(const char* path);
        void close();
    };

    using FileMappingPtr = Pointer<FileMapping>;
    using FileMappingRef = RefPointer<FileMapping>;
    using NewFileMapping = NewPointer<FileMapping>;

    /**/

    FileMapping::FileMapping()
    {
        reset();
    }

    FileMapping::FileMapping(String* path)
    {
        reset();
        if (!open(path->c_str()))
            UNUSED(path);
    }

    FileMapping::~FileMapping()
    {
        close();
    }

    /**/

    inline bool FileMapping::isopened() noexcept
    {
        return opened;
    }

    inline void FileMapping::reset() noexcept
    {
        opened = false;
#if WASMGEN_TARGET_UNIX
        fd = -1;
#elif WASMGEN_TARGET_WINDOWS
        m_hFile = INVALID_HANDLE_VALUE;
        m_hMap = NULL;
#endif
    }

    bool FileMapping::open(const char* path)
    {
#if WASMGEN_TARGET_UNIX

        fd = ::open(path, O_RDONLY);
        if (fd < 0)
        {
            WASMGEN_DEBUG(2, "open failed: ", path, "\n");
            return false;
        }

        struct stat stbuf;

        if (::fstat(fd, &stbuf) < 0)
        {
            close();
            return false;
        }

        data_size = size_t(stbuf.st_size);
        if (data_size == 0)
        {
            opened = true;
            data_addr = nullptr;
            return true;
        }

        int mflags = MAP_FILE | MAP_PRIVATE;

        data_addr = ::mmap(nullptr, data_size, PROT_READ, mflags, fd, 0);
        if (data_addr == MAP_FAILED)
        {
            close();
            return false;
        }

        opened = true;
        return true;

#elif WASMGEN_TARGET_WINDOWS

        auto wpath = utfcvt::to_wstring(path);
        m_hFile = ::CreateFileW(wpath.c_str(),
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                NULL,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                NULL);
        if (m_hFile == INVALID_HANDLE_VALUE)
            return false;

        LARGE_INTEGER fsize;
        if (!::GetFileSizeEx(m_hFile, &fsize))
        {
            close();
            return false;
        }

        data_size = size_t((uint64_t(fsize.HighPart) << 32) + fsize.LowPart);
        if (data_size == 0)
        {
            opened = true;
            data_addr = NULL;
            return true;
        }

        m_hMap = ::CreateFileMappingA(m_hFile,
                                      NULL,
                                      PAGE_READONLY,
                                      fsize.HighPart,
                                      fsize.LowPart,
                                      NULL);
        if (m_hMap == NULL)
        {
            close();
            return false;
        }

        data_addr = ::MapViewOfFile(m_hMap, FILE_MAP_READ, 0, 0, data_size);
        if (data_addr == NULL)
        {
            close();
            return false;
        }

        opened = true;
        return true;

#endif
    }

    void FileMapping::close()
    {
#if WASMGEN_TARGET_UNIX

        if (data_size > 0 && ::munmap((void*)data_addr, data_size) < 0)
            WASMGEN_DEBUG(2, "munmap failed: ", data_addr, ":", data_size, "\n");
        if (fd >= 0 && ::close(fd) < 0)
            WASMGEN_DEBUG(2, "close failed: ", fd, "\n");
        fd = -1;

#elif WASMGEN_TARGET_WINDOWS

        if (data_size > 0 && !::UnmapViewOfFile(data_addr))
            WASMGEN_DEBUG(2, "UnmapViewOfFile failed: ", data_addr, "\n");
        if (m_hMap != NULL && !::CloseHandle(m_hMap))
            WASMGEN_DEBUG(2, "CloseHandle failed: ", m_hMap, "\n");
        if (m_hFile != INVALID_HANDLE_VALUE && !::CloseHandle(m_hFile))
            WASMGEN_DEBUG(2, "CloseHandle failed: ", m_hFile, "\n");
        m_hFile = INVALID_HANDLE_VALUE;
        m_hMap = NULL;

#endif

        opened = false;
        data_addr = nullptr;
        data_size = 0;
    }

    ////////////////
    // FileReader //
    ////////////////

    FileReader::FileReader(String* path)
        : name(&FileString::null)
        , encode(&FileString::null)
        , data()
        , loaded(false)
    {
        if (*path != "-")
        {
            name = path;

            NewFileMapping fm(path);

            if (!fm->isopened())
                return;
            data = &fm;
        }
        else
        {
            name = new String("<stdin>");
            data = new FileStreamData(stdin);
        }
        loaded = true;

        WASMGEN_DEBUG(2,
                      "File: ", path->c_str(),
                      "  [", data->address(), ", ",
                      {"0x", std::hex, data->size()}, "]",
                      "\n");
    }

    FileReader::FileReader(FileData* data, String* name)
        : name(name ? name : &FileString::null)
        , encode(&FileString::null)
        , data(data)
        , loaded(true)
    {
        /*NOOP*/
    }

    ////////////////////
    // TextFileReader //
    ////////////////////

    TextFileReader::TextFileReader(String* path)
        : super(path)
    {
        initialize();
    }

    TextFileReader::TextFileReader(FileData* data, String* name)
        : super(data, name)
    {
        initialize();
    }

    void TextFileReader::initialize()
    {
        text_size = 0;
        text_remain = 0;

        text_top = nullptr;
        text_end = nullptr;
        text_pos = nullptr;

        replace = 0;
        getcode = nullptr;
        line = 0;

        if (!valid())
            return;

        const uint8_t *p = (const uint8_t*)data->address();

        text_size = data->size();
        text_top = p;
        text_end = text_top + text_size;
        text_pos = text_top;

        uint16_t bom = 0xfeff;
        size_t align = 1;

        switch (text_size)
        {
        default:
            if (LittleEndian::read<uint32_t>(p) == bom)
            {   // UTF-32 LE
                encode = FileString::utf32le;
                getcode = utfcvt::utf::nextcode<uint32le_t>;
                replace = utfcvt::replace_u32c();
                p += 4;
                align = 4;
                break;
            }
            if (BigEndian::read<uint32_t>(p) == bom)
            {   // UTF-32 BE
                encode = FileString::utf32be;
                getcode = utfcvt::utf::nextcode<uint32be_t>;
                replace = utfcvt::replace_u32c();
                p += 4;
                align = 4;
                break;
            }
            fallthrough;
        case 3:
            if (p[0] == 0xef && p[1] == 0xbb && p[2] == 0xbf)
            {   // UTF-8
                encode = FileString::utf8_bom;
                getcode = utfcvt::utf::nextcode<uint8_t>;
                replace = utfcvt::replace_u8c();
                p += 3;
                align = 1;
                break;
            }
            fallthrough;
        case 2:
            if (LittleEndian::read<uint16_t>(p) == bom)
            {   // UTF-16 LE
                encode = FileString::utf16le;
                getcode = utfcvt::utf::nextcode<uint16le_t>;
                replace = utfcvt::replace_u16c();
                p += 2;
                align = 2;
                break;
            }
            if (BigEndian::read<uint16_t>(p) == bom)
            {   // UTF-16 BE
                encode = FileString::utf16be;
                getcode = utfcvt::utf::nextcode<uint16be_t>;
                replace = utfcvt::replace_u16c();
                p += 2;
                align = 2;
                break;
            }
            fallthrough;
        case 1:
        case 0:
            {   // default: UTF-8
                encode = FileString::utf8;
                getcode = utfcvt::utf::nextcode<uint8_t>;
                replace = utfcvt::replace_u8c();
                // p += 0;
                align = 1;
                break;
            }
        }

        text_remain = (text_end - (const uint8_t*)text_pos) & (align - 1);
        text_top = p;
        text_end -= text_remain;
        text_pos = p;
    }

    /**/

    std::u32string TextFileReader::full()
    {
        const void* save = text_pos;
        std::u32string text;

        text_pos = text_top;
        while (remain())
        {
            FileGetCRes res = getchar();

            text += res.success ? res.code : replace;
        }
        text_pos = save;

        return text;
    }

    void TextFileReader::show_info()
    {
        verbose({ "File: ", c_path(), "\n" });
        WASMGEN_DEBUG(1, "  text encoding: ", c_encoding(), "\n");
        WASMGEN_DEBUG(1, "  data: [", address(), ", ", {"0x", std::hex, size()}, "]\n");
        if (flag_debug >= 4)
            message(utfcvt::to_string(full()));
    }

    ////////////////
    // FileWriter //
    ////////////////

    FileWriter::FileWriter(String* path)
        : name(*FileString::null)
        , handle(nullptr)
    {
        if (*path == "-")
        {
            handle = stdout;
            return;
        }

        const char* fname = path->c_str();

#if !WASMGEN_TARGET_WINDOWS

        handle = ::fopen(fname, "wb");
        if (!handle)
            ::fprintf(stderr, "open error: %s\n", fname);

#else  /* WINDOWS */

        errno_t error = ::fopen_s(&handle, fname, "wb");

        if (error)
            ::fprintf(stderr, "open error(%d): %s\n", error, fname);

#endif
    }

    FileWriter::~FileWriter()
    {
        close();
    }

    bool FileWriter::write(const ByteArray& data) noexcept
    {
        if (!handle)
            return false;

        size_t dsz = data.size();
        size_t wsz = ::fwrite(&data[0], 1, dsz, handle);
        return dsz == wsz;
    }

} // wasmgen
