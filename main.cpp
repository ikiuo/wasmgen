/*
 * main.cpp
 */
#include "parser.hpp"
#include "wmain.hpp"

#define WASMGEN_VERSION_MAJOR    0
#define WASMGEN_VERSION_MINOR    1
#define WASMGEN_VERSION_RELEASE  1
#define WASMGEN_VERSION_STRING   "0.1.1"

using namespace wasmgen;
using EnvMap = StdStringMap<StringPtr>;

/**/

static EnvMap environment;

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
        message({"senv:", senv->c_str(), "\n"});

        StringListRef ienv(senv->split(WASMGEN_TARGET_WINDOWS ? ';' : ':'));

        includes->insert(includes->end(), ienv->begin(), ienv->end());
    }
}

/**/

static bool parse_asmset(StdStringList& asmset)
{
    const StdString prefix("@set ");
    StdString text;

    for (auto& s : asmset)
        text += StdString(prefix + s + "\n");

    NewTextFileReader reader(new FileStreamData(text),
                             new String("<command-line>"));
    NewParser parser(reader);

    if (!parser->parse())
        return false;
    parser->update_clset();
    return true;
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
    flag_debug = level;

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

static int usage()
{
    message("WebAssembly 用アセンブラ Version " WASMGEN_VERSION_STRING "\n"
            "\n"
            "使用方法: ", program, " [オプション] INPUT OUTPUT\n"
            "\n"
            "オプション:\n"
            "    -q      静寂モード.\n"
            "    -v      冗長モード.\n"
            "\n"
            "    --debug=level\n"
            "            デバッグ レベルを設定する.\n"
            "    -d, --debug\n"
            "            デバッグ レベルを一つ増やす.\n"
            "\n"
            "    -n, --no-wasm\n"
            "            非出力モード.\n"
            "\n"
            "    -D <name>=<value>\n"
            "            アセンブラ変数を設定する.(@set <name>=<value>)\n"
            "    -I <path>\n"
            "            @include 用検索パスを追加する.\n"
            "\n");
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
    auto new_argc = 0;
    auto new_argv = argv;
    auto flag_nowasm = false;
    StdStringList asmset;
    StringPtr input;
    StringPtr output;

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

            if (!strcmp(arg, "--no-wasm"))
            {
                flag_nowasm = true;
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
                continue;

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
                flag_nowasm = true;
                continue;

            default:
                return usage();
            }
            break;
        }
    }

    argc = new_argc;
    argv = new_argv;

    if (argc != (flag_nowasm ? 1 : 2))
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
        message({"open failed: ", freader->c_path(), "\n"});
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

    if (flag_nowasm)
        return 0;

    output = new String(argv[1]);

    verbose({ "Output: ", output->c_str(), "\n" });

    NewFileWriter fwriter(&output);

    if (!fwriter->write(binary))
    {
        message({"Write error: ", output->c_str(), "\n"});
        return 2;
    }
    return 0;
}
