#include "CLOptions.h"
#include "xrMemory.h"
#include "log.h"

template<> xr_list<CLOption<bool> *> CLOption<bool>::options = {};
template<> xr_list<CLOption<int> *> CLOption<int>::options = {};
template<> xr_list<CLOption<pstr> *> CLOption<pstr>::options = {};

// CLOption

// when adding make sure it's beginning with a '-'
template<typename T>
CLOption<T>::CLOption(pcstr opname, pcstr desc, const T defval, bool req)
    : provided(false), required(req)
{
    if(!IsOptionFlag(opname))
    {
        size_t opsz = xr_strlen(opname) + 1;
        option_name = xr_alloc<char>(opsz);
        option_name[0] = '-';
        xr_strcpy(&option_name[1], opsz - 1, opname);
    }
    else
    {
        option_name = xr_strdup(opname);
    }
    description = xr_strdup(desc);
    copy_argument(defval);
    options.push_back(this);
}


template<typename T>
void CLOption<T>::copy_argument(T arg) {
    argument = arg;
}

template<>
void CLOption<pstr>::copy_argument(pstr arg) {
    argument = xr_strdup(arg);
}

template<typename T>
void CLOption<T>::free_argument() {
    return;
}

template<>
void CLOption<pstr>::free_argument() {
    xr_free(argument);
}


template<typename T>
typename xr_list<CLOption<T> *>::iterator CLOption<T>::find_option(pcstr flag_name)
{
    typename xr_list<CLOption<T> *>::iterator it;
    for(it = CLOption<T>::options.begin(); it != CLOption<T>::options.end(); it++)
    {
        if (!xr_strcmp((*it)->option_name, flag_name)) break;
    }
    return it;
}

template<typename T>
CLOption<T>::~CLOption()
{
    auto it = find_option(option_name);
    if (it != options.end()) options.erase(it);
    xr_free(option_name);
    xr_free(description);
    free_argument();
}

template<typename T>
bool CLOption<T>::IsProvided() const
{
    return provided;
}

template<typename T>
T CLOption<T>::OptionValue() const
{
    return argument;
}

template<typename T>
void CLOption<T>::CheckArguments()
{
    typename xr_list<CLOption<T> *>::iterator it;
    for(it = CLOption<T>::options.begin(); it != CLOption<T>::options.end(); it++)
    {
        if((*it)->required && !(*it)->provided)
        {
            throw CLOptionMissing((*it)->option_name);
        }
    }
}

template<>
bool CLOption<bool>::parse_option(pcstr option, pcstr arg)
{
    auto it = CLOption<bool>::find_option(option);
    if(it == CLOption<bool>::options.end()) return false; // not found

    // found -> must be of type bool
    CLOption<bool> *o = *it;

    o->argument = true;
    o->provided = true;
    return true;                // found and set
}

template<>
bool CLOption<int>::parse_option(pcstr option, pcstr arg)
{
    auto it = CLOption<int>::find_option(option);
    if(it == CLOption<int>::options.end()) return false; // not found

    // found -> must be of type int
    CLOption<int> *o = *it;

    o->argument = std::stoi(arg); // may throw
    o->provided = true;
    return true;
}

template<>
bool CLOption<pstr>::parse_option(pcstr option, pcstr arg)
{
    auto it = CLOption<pstr>::find_option(option);
    if(it == CLOption<pstr>::options.end()) return false; // not found

    // found -> must be of type string
    CLOption<pstr> *o = *it;

    o->argument = xr_strdup(arg);
    o->provided = true;
    return true;
}

template<typename T>
void CLOption<T>::PrintHelp() {
    typename xr_list<CLOption<T> *>::iterator it;
    for(it = CLOption<T>::options.begin(); it != CLOption<T>::options.end(); it++) {
        pcstr isreq = (*it)->required ? "(mandatory)" : "(optional)";
        Msg("%-10s \t %-10s \t %-25s", (*it)->option_name, isreq, (*it)->description);
    }    
}

void ParseCommandLine(int argc, char **argv)
{
    // put these back into class methods?
    for(int n = 1; n < argc; n++)
    {
        if(!IsOptionFlag(argv[n]))
        {
            Msg("Unknown option/argument <%s>", argv[n]);
            continue;
        }

        // is this a bool option?
        if(CLOption<bool>::parse_option(argv[n], nullptr))
        {
            continue;
        }
        // the rest of the flags will require an argument
        else if((n + 1 >= argc) || IsOptionFlag(argv[n + 1]))
        {
            throw CLOptionParam(argv[n]);
        }
        else if(CLOption<int>::parse_option(argv[n], argv[n + 1])
                || CLOption<pstr>::parse_option(argv[n], argv[n + 1]))
        {
            n++;
            continue;
        }

        Msg("Unknown option <%s>", argv[n]);
    }
}


void CLCheckAllArguments()
{
    CLOption<bool>::CheckArguments();
    CLOption<int>::CheckArguments();
    CLOption<pstr>::CheckArguments();
}

XRCORE_API void CLPrintAllHelp() {
    CLOption<bool>::PrintHelp();
    CLOption<int>::PrintHelp();
    CLOption<pstr>::PrintHelp();
}

template class XRCORE_API CLOption<bool>;
template class XRCORE_API CLOption<int>;
template class XRCORE_API CLOption<pstr>;
