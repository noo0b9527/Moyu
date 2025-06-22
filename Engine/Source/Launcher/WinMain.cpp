#include <assert.h>
#include <windows.h>

#include <iostream>
#include <string>

#include "DynamicLibrary.h"

using RuntimeMain = int (*)(HINSTANCE, wchar_t const*);

#define CONSOLE

#if !defined(CONSOLE)
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int)
{
    wchar_t hostPath[MAX_PATH];
    DWORD   size = GetModuleFileNameW(nullptr, hostPath, MAX_PATH);
    assert(size != 0 && size < MAX_PATH);

    std::basic_string<wchar_t> rootPath = hostPath;
    auto                       pos      = rootPath.find_last_of(L"\\");
    assert(pos != std::basic_string<wchar_t>::npos);
    rootPath = rootPath.substr(0, pos + 1);
#else
int __cdecl wmain(int, wchar_t* argv[])
{
    HINSTANCE instance = GetModuleHandle(nullptr);
    wchar_t   hostPath[MAX_PATH];
    auto size = GetFullPathNameW(argv[0], sizeof(hostPath) / sizeof(wchar_t),
                                 hostPath, nullptr);
    assert(size != 0);

    std::basic_string<wchar_t> rootPath = hostPath;
    auto                       pos      = rootPath.find_last_of(L"\\");
    assert(pos != std::basic_string<wchar_t>::npos);
    rootPath = rootPath.substr(0, pos + 1);
#endif

    DynamicLibrary library("Runtime");
    auto runtimeMain = library.GetFunction<RuntimeMain>("RuntimeMain");
    assert(runtimeMain != nullptr &&
           "Failed to Get Function By Name RuntimeMain");

    return runtimeMain(instance, rootPath.c_str());
}
