#pragma once

// #include <cstdint>
// #include <cstdio>
// #include <cstdarg>

#include <format>
#include <string>

#include "Runtime/Base/Include/Base/_Module/API.h"

// Use EASTL???

namespace Moyu
{
    using String  = std::basic_string<char>;
    using WString = std::basic_string<wchar_t>;

    template<typename... Args>
    inline size_t Printf(char* pBuffer, uint32_t bufferSize,
                         std::format_string<Args...> fmt, Args&&... args)
    {
        auto result = std::format_to_n(pBuffer, bufferSize - 1, fmt,
                                       std::forward<Args>(args)...);
        *result.out = '\0';
        return result.size;
    }

} // namespace Moyu

namespace Moyu::StringUtils
{} // namespace Moyu::StringUtils
