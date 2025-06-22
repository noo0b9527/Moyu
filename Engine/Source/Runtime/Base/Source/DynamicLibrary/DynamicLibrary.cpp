#include "Base/DynamicLibrary.h"

#include "Base/Config.h"

#if defined(MOYU_PLATFORM_WINDOWS)
#include <windows.h>
#define LIB_EXT STR(".dll")
#else
#include <dlfcn.h>
#include <limits.h>
#if defined(__APPLE__)
#define LIB_EXT STR(".dylib")
#else
#define LIB_EXT STR(".so")
#endif
#endif

namespace Moyu
{
    DynamicLibrary::DynamicLibrary(WString& libraryPath)
    {
        WString fullName = libraryPath;
        if (libraryPath.find_last_of('.') == std::string::npos)
        {
            fullName += LIB_EXT;
        }
#if defined(MOYU_PLATFORM_WINDOWS)
        m_handle = LoadLibraryW(fullName.c_str());
#else
        m_handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
#endif
        assert(m_handle != nullptr && "Failed to load dynamic library");
    }

    DynamicLibrary::DynamicLibrary(wchar const* libraryPath)
    {
#if defined(MOYU_PLATFORM_WINDOWS)
        m_handle = LoadLibraryW(libraryPath);
#else
        m_handle = dlopen(path, RTLD_LAZY | RTLD_LOCAL);
#endif
        assert(m_handle != nullptr && "Failed to load dynamic library");
    }

    void DynamicLibrary::FreeLibrary(void* handle)
    {
        if (handle)
        {
#if defined(MOYU_PLATFORM_WINDOWS)
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            FreeLibrary(handle);
#endif
        }
    }

    void* DynamicLibrary::GetExport(void* h, const char* name)
    {
        void* symbol = nullptr;
#if defined(MOYU_PLATFORM_WINDOWS)
        symbol = GetProcAddress(static_cast<HMODULE>(h), name);
#else
        symbol = dlsym(h, name);
#endif
        assert(symbol != nullptr && "Failed to get export symbol");
        return symbol;
    }
} // namespace Moyu
