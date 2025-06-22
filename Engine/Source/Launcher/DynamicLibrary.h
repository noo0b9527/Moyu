#pragma once

#include <assert.h>

#include <iostream>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define LAUNCHER_LIB_HANDLE HINSTANCE
#define LAUNCHER_LIB_LOAD(name) LoadLibraryA(name)
#define LAUNCHER_LIB_GETSYM(handle, sym) GetProcAddress(handle, sym)
#define LAUNCHER_LIB_CLOSE(handle) FreeLibrary(handle)
#define LAUNCHER_LIB_EXT ".dll"
#else
#include <dlfcn.h>
#define LAUNCHER_LIB_HANDLE void*
#define LAUNCHER_LIB_LOAD(name) dlopen(name, RTLD_LAZY)
#define LAUNCHER_LIB_GETSYM(handle, sym) dlsym(handle, sym)
#define LAUNCHER_LIB_CLOSE(handle) dlclose(handle)
#if defined(__APPLE__)
#define LAUNCHER_LIB_EXT ".dylib"
#else
#define LAUNCHER_LIB_EXT ".so"
#endif
#endif

class DynamicLibrary
{
public:
    explicit DynamicLibrary(const std::string& libraryName)
    {
        std::string fullName = libraryName;
        if (libraryName.find_last_of('.') == std::string::npos)
        {
            fullName += LAUNCHER_LIB_EXT;
        }

        m_handle = LAUNCHER_LIB_LOAD(fullName.c_str());
        assert(m_handle != nullptr && "Failed to load dynamic library");

        if (!m_handle)
        {
            std::cerr << "Failed to load library " << fullName << ": ";
#if defined(_WIN32) || defined(_WIN64)
            DWORD errorCode = GetLastError();
            char  buffer[256];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                buffer, sizeof(buffer), nullptr);
            std::cerr << buffer << std::endl;
#else
            std::cerr << dlerror() << std::endl;
#endif
            std::abort();
        }
    }

    DynamicLibrary(const DynamicLibrary&)            = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    DynamicLibrary(DynamicLibrary&& other) noexcept : m_handle(other.m_handle)
    {
        other.m_handle = nullptr;
    }

    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept
    {
        if (this != &other)
        {
            if (m_handle)
            {
                LAUNCHER_LIB_CLOSE(m_handle);
            }
            m_handle       = other.m_handle;
            other.m_handle = nullptr;
        }
        return *this;
    }

    ~DynamicLibrary()
    {
        if (m_handle)
        {
            LAUNCHER_LIB_CLOSE(m_handle);
        }
    }

    template<typename FuncPtr>
    FuncPtr GetFunction(const std::string& functionName) const
    {
        assert(m_handle != nullptr && "Library not loaded");

#if !defined(_WIN32) && !defined(_WIN64)
        dlerror();
#endif

        auto symbol = reinterpret_cast<FuncPtr>(
            LAUNCHER_LIB_GETSYM(m_handle, functionName.c_str()));

#if !defined(_WIN32) && !defined(_WIN64)
        const char* error = dlerror();
        if (error)
        {
            std::cerr << "Failed to get symbol " << functionName << ": "
                      << error << std::endl;
            std::abort();
        }
#else
        if (!symbol)
        {
            DWORD errorCode = GetLastError();
            if (errorCode != 0)
            {
                char buffer[256];
                FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
                                   FORMAT_MESSAGE_IGNORE_INSERTS,
                               nullptr, errorCode,
                               MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                               buffer, sizeof(buffer), nullptr);
                std::cerr << "Failed to get symbol " << functionName << ": "
                          << buffer << std::endl;
                std::abort();
            }
        }
#endif

        assert(symbol != nullptr && "Failed to get function pointer");
        return symbol;
    }

    bool IsLoaded() const
    {
        return m_handle != nullptr;
    }

private:
    LAUNCHER_LIB_HANDLE m_handle = nullptr;
};
