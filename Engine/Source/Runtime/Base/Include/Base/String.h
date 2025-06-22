#pragma once

#include <string>


#include "Base/Config.h"

#if defined(MOYU_PLATFORM_WINDOWS)
#define STR(s) L##s
#define CH(c) L##c
#define DIR_SEPARATOR L'\\'
#define string_compare wcscmp
using wchar = wchar_t;
#else
#define STR(s) s
#define CH(c) c
#define DIR_SEPARATOR '/'
#define MAX_PATH PATH_MAX
#define string_compare strcmp
using wchar = char;
#endif

namespace Moyu
{
    using WString = std::basic_string<wchar>;
    using String = std::basic_string<char>;
}
