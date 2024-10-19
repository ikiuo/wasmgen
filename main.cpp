/*
 * main.cpp
 */
#include "parser.hpp"
#include "wmain.hpp"

#define WASMGEN_VERSION_MAJOR    0
#define WASMGEN_VERSION_MINOR    2
#define WASMGEN_VERSION_RELEASE  7
#define WASMGEN_VERSION_STRING   "0.2.7"

using namespace wasmgen;
using StrKeyValue = StdStringMap<StringPtr>;

/**/

static StrKeyValue environment;

static void init_environment(char **envp)
{
    for (char* ep = *envp++; ep; ep = *envp++)
    {
        StringListRef env(String(ep).split('=', 1));

        if (env->size() == 2)
            environment[*(*env)[0]] = Transfer((*env)[1]);
    }
}

static String* get_environment(const char* name)
{
    auto it = environment.find(name);

    return it != environment.end() ? &it->second : nullptr;
}

static void get_wasmgen_include()
{
    StringPtr senv(get_environment("WASMGEN_INCLUDE"));

    if (senv)
    {
        message("senv:", senv->c_str(), "\n");

        StringListRef ienv(senv->split(WASMGEN_TARGET_WINDOWS ? ';' : ':'));

        includes->insert(includes->end(), ienv->begin(), ienv->end());
    }
}

/**/

static int opt_setdebug(char* debug)
{
    if (!debug)
        return 1;

    unsigned long level = ::strtoul(debug, NULL, 10);

    if (!level && errno == ERANGE)
    {
        message("invalid debug level: ", debug, "\n");
        return 2;
    }
    flag_debug = int(level);

    return 0;
}

static int opt_setlocale(char* locale)
{
    if (!locale)
        return 1;
    if (!::setlocale(LC_ALL, locale))
        message("invalid locale: ", locale, "\n");
    return 0;
}

/**/

static Parser* parse_text(const StdString& text, const char* filename)
{
    NewTextFileReader reader(new FileStreamData(text), new String(filename));
    NewParser parser(reader);

    return parser->parse() ? Finish(parser) : nullptr;
}


/**/

static bool parse_asmset(StdStringList& asmset)
{
    const StdString prefix("@set ");
    StdString text;

    for (auto& s : asmset)
        text += StdString(prefix + s + "\n");

    ParserRef parser = parse_text(text, "<command-line>");

    if (!parser)
        return false;
    parser->update_clset();
    return true;
}

/**/

struct Base64Option
{
    int indent;
    int quote;
    int width;
    int comma;
    bool crlf;

    Base64Option()
        : indent(0)
        , quote(0)
        , width(0)
        , comma(0)
        , crlf(WASMGEN_TARGET_WINDOWS)
        { /*NOOP*/ }
};

static bool parse_base64opt(Base64Option& out, const char* arg)
{
    const StdString prefix("@set ");
    NewString equal(" = ");
    NewString option(arg);
    StringListRef opts = option->split(',');
    StdString text;

    for (auto opt : *opts)
    {
        StringListRef kv = opt->split('=', 1);

        if (kv->size() == 1)
            kv->push_back(new String("1"));

        StringRef value = equal->join(kv);

        text += StdString(prefix + *value + "\n");
    }

    ParserRef parser = parse_text(text, "<base64-option>");

    if (!parser)
        return false;

    ExprValue indent = parser->get_asmsw_value("i");
    ExprValue quote = parser->get_asmsw_value("q");
    ExprValue width = parser->get_asmsw_value("w");
    ExprValue comma = parser->get_asmsw_value("c");

    if (indent.isvalid())
    {
        out.indent = indent;
        if (!indent.isnumber() || out.indent < 0)
        {
            puterr("i オプションが不正です。");
            return false;
        }
    }
    if (quote.isvalid())
    {
        out.quote = quote;
        if (!quote.isnumber() || out.quote < 0 || 2 < out.quote)
        {
            puterr("q オプションが不正です。");
            return false;
        }
    }
    if (width.isvalid())
    {
        out.width = width;
        if (!width.isnumber() || out.width <= 0)
        {
            puterr("w オプションが不正です。");
            return false;
        }
    }
    if (comma.isvalid())
    {
        out.comma = comma;
        if (!comma.isnumber() || out.comma < 0 || 1 < out.comma)
        {
            puterr("c オプションが不正です。");
            return false;
        }
    }
    return true;
}

static ByteArray base64_encode(const ModuleBinary& binary, Base64Option& option)
{
    ByteArray base64(binary.base64());
    const uint8_t* data = base64.data();
    int length = int(base64.size());

    ByteArray indent(option.indent, ' ');
    uint8_t quote = "\0\'\""[option.quote];
    int width = option.width;
    uint8_t comma = "\0,"[option.comma];
    ByteArray newline;

    ByteArray text;

    if (option.crlf)
        newline.push_back('\r');
    newline.push_back('\n');

    if (width == 0)
        width = length;

    int pos = 0;

    for (pos = 0; (pos + width) < length; pos += width)
    {
        text.insert(text.end(), indent.begin(), indent.end());
        if (quote)
            text.push_back(quote);
        text.insert(text.end(), data + pos, data + pos + width);
        if (quote)
            text.push_back(quote);
        if (comma)
            text.push_back(comma);
        text.insert(text.end(), newline.begin(), newline.end());
    }
    if (pos < length)
    {
        text.insert(text.end(), indent.begin(), indent.end());
        if (quote)
            text.push_back(quote);
        text.insert(text.end(), data + pos, data + length);
        if (quote)
            text.push_back(quote);
        if (comma)
            text.push_back(comma);
        text.insert(text.end(), newline.begin(), newline.end());
    }
    return text;
}

/**/

static int usage()
{
    message(
        "WebAssembly 用アセンブラ Version " WASMGEN_VERSION_STRING "\n"
        "\n"
        "使用方法: ", program, " [オプション] 入力ファイル 出力ファイル\n"
        "\n"
        "入力ファイル  wasmgen 形式ソース ファイル名\n"
        "    \"-\" で標準入力\n"
        "\n"
        "出力ファイル  生成するファイル名\n"
        "    \"-\" で標準出力\n"
        "    オプションによる Base64 符号化あり\n"
        "\n"
        "オプション:\n"
        "    -D <name>=<value>\n"
        "            アセンブラ変数を設定(@set <name>=<value>)\n"
        "    -I <path>\n"
        "            @include 用検索パスを追加\n"
        "\n"
        "    -n, --no-output\n"
        "            非出力モード(出力ファイルは無視)\n"
        "    -A, --base64\n"
        "            Base64形式モード\n"
        "    --base64+OPTION\n"
        "            Base64形式モード(追加オプションあり)\n"
        "\n"
        "    -q      静寂モード\n"
        "    -v      冗長モード\n"
        "\n"
        "    --debug=level\n"
        "            デバッグ レベルを設定\n"
        "    -d, --debug\n"
        "            デバッグ レベルを増加\n"
        "\n"
        "\n"
        "Base64形式追加オプション:\n"
        "    形式: --base64+key1,key2=val2,...\n"
        "        値(=val部分)を省略すると\"1\"になる\n"
        "        i=N : インデント(空白文字数)を指定\n"
        "        q=N : 括り文字の指定\n"
        "            N=0 なし\n"
        "            N=1 シングル クォート(')\n"
        "            N=2 ダブル クォート(\")\n"
        "        w=N : 一行分の文字数を指定\n"
        "        c=N : 行末のコンマ(,)を指定\n"
        "            N=0 なし\n"
        "            N=1 あり\n"
        "\n"
        );
    return 1;
}

static char* shift_arg(int& argc, char**& argv)
{
    char* arg = NULL;

    if (argc > 0)
    {
        arg = *argv;
        --argc;
        ++argv;
    }
    return arg;
}

#define shift()  (shift_arg(argc, argv))

int main(int argc, char** argv, char** envp)
{
    const char* opt_base64_name = "--base64";
    size_t opt_base64_name_len = strlen(opt_base64_name);

    auto new_argc = 0;
    auto new_argv = argv;
    auto flag_nowrite = false;
    auto flag_base64 = false;
    StdStringList asmset;
    StringPtr input;
    StringPtr output;
    Base64Option base64opts;

    init_environment(envp);

    program = shift();
    while (argc > 0)
    {
        auto arg = shift();

        if ((arg[0] != '-') || (arg[1] == 0))
        {
            new_argv[new_argc++] = arg;
            continue;
        }
        if (arg[1] == '-')
        {
            if (!strcmp(arg, "--locale"))
            {
                switch (opt_setlocale(shift()))
                {
                case 0: break;
                case 1: return usage();
                default: return 2;
                }
                continue;
            }
            if (!strcmp(arg, "--quiet"))
            {
                flag_quiet = true;
                continue;
            }
            if (!strcmp(arg, "--verbose"))
            {
                flag_verbose = true;
                continue;
            }
            if (!strcmp(arg, "--debug"))
            {
                ++flag_debug;
                continue;
            }
            if (!strncmp(arg, "--debug=", 8))
            {
                switch (opt_setdebug(arg + 8))
                {
                case 0: break;
                case 1: return usage();
                default: return 2;
                }
                continue;
            }

            if (!strcmp(arg, "--no-output"))
            {
                flag_nowrite = true;
                continue;
            }
            if (!strncmp(arg, opt_base64_name, opt_base64_name_len))
            {
                flag_base64 = true;
                if (!arg[opt_base64_name_len])
                    continue;
                if (arg[opt_base64_name_len] != '+')
                    return usage();
                if (!parse_base64opt(base64opts, arg + opt_base64_name_len + 1))
                    return usage();
                continue;
            }

            return usage();
        }

        int s_opt;

        while ((s_opt = *(++arg)) != 0)
        {
            switch (s_opt)
            {
            case 'l':
                switch (opt_setlocale(shift()))
                {
                case 0: continue;
                case 1: return usage();
                default: return 2;
                }

            case 'D':
                {
                    auto asmsw = ++arg;

                    if (!*asmsw)
                        if (!(asmsw = shift()))
                            return usage();
                    asmset.push_back(asmsw);
                }
                break;

            case 'I':
                {
                    auto ipath = ++arg;

                    if (!*ipath)
                        if (!(ipath = shift()))
                            return usage();
                    includes->push_back(new String(ipath));
                }
                break;

            case 'd':
                ++flag_debug;
                continue;
            case 'q':
                flag_quiet = true;
                continue;
            case 'v':
                flag_verbose = true;
                continue;
            case 'n':
                flag_nowrite = true;
                continue;
            case 'A':
                flag_base64 = true;
                continue;

            default:
                return usage();
            }
            break;
        }
    }

    argc = new_argc;
    argv = new_argv;

    if (argc < (flag_nowrite ? 1 : 2))
        return usage();

    WASMGEN_DEBUG(1, "DEBUG: level = ", flag_debug, "\n");

    if (!parse_asmset(asmset))
        return 2;

    get_wasmgen_include();

    if (flag_debug >= 4)
        Token::dump_charset();

    input = new String(argv[0]);

    NewTextFileReader freader(&input);

    if (!freader->valid())
    {
        message("open failed: ", freader->c_path(), "\n");
        return 2;
    }
    freader->show_info();

    NewParser parser(freader);

    try
    {
        if (!parser->parse())
            return 2;
    }
    catch (BaseException& e)
    {
        std::cerr << e << std::endl;
        return 2;
    }

    ModuleBinary binary;

    parser->build_wasm(binary);

    if (flag_nowrite)
        return 0;

    if (flag_base64)
        (ByteArray&)binary = base64_encode(binary, base64opts);

    output = new String(argv[1]);

    verbose("Output: ", output->c_str(), "\n");

    NewFileWriter fwriter(&output);

    if (!fwriter->write(binary))
    {
        message("Write error: ", output->c_str(), "\n");
        return 2;
    }
    return 0;
}
