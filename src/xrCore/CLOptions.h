#pragma once

#if !defined(_CPPUNWIND)
#error Please enable exceptions...
#endif

#include <exception>

#include "xrCommon/xr_list.h"
#include "_std_extensions.h"

template<typename T> class XRCORE_API CLOption;

template<typename T>
class XRCORE_API CLOption
{
public:
    CLOption(pcstr flag_name, pcstr desc, const T defval, bool req = false);
    ~CLOption();
    bool IsProvided() const;          // was the option provided in the CLI?
    T OptionValue() const;            // value provided with the option

    static void CheckArguments();
    static void PrintHelp();
    friend void ParseCommandLine(int argc, char *argv[]);

private:
    pstr option_name = nullptr;
    pstr description = nullptr;
    bool required;
    bool provided = false;

    T argument;

    void copy_argument(T arg);
    void free_argument();
    static typename xr_list<CLOption<T> *>::iterator find_option(pcstr flag_name);
    static bool parse_option(pcstr option, pcstr arg);
    static xr_list<CLOption<T> *> options;
};

// exception

struct CLOptionMissing : public std::exception
{
    char *option_error = nullptr;

    CLOptionMissing(pstr opt_err) { option_error = xr_strdup(opt_err); }
    ~CLOptionMissing() { xr_free(option_error); }
    const char * what() const throw()
    {
        return option_error; // missing argument
    }
};

struct CLOptionParam : public std::exception
{
    char *option_error = nullptr;

    CLOptionParam(pstr opt_err) { option_error = xr_strdup(opt_err); }
    ~CLOptionParam() { xr_free(option_error); }
    const char * what() const throw()
    {
        return option_error; // missing argument parameter
    }
};

inline static bool IsOptionFlag(const char *buf)
{
    return (buf && buf[0] == '-');
}

XRCORE_API void ParseCommandLine(int argc, char **argv);
XRCORE_API void CLCheckAllArguments();
XRCORE_API void CLPrintAllHelp();
