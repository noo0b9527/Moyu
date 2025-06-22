#include <iostream>

#include "Base/Config.h"
#include "Base/Log.h"

#include "Module/Script.h"

#include "Runtime/_Module/API.h"

#if defined(MOYU_PLATFORM_WINDOWS)
#include <windows.h>
extern "C"
{
    MOYUAPI int MOYUCALL RuntimeMain(HINSTANCE instance, wchar_t const* rootPath);
}
#endif

#if defined(MOYU_PLATFORM_WINDOWS)
int RuntimeMain(HINSTANCE instance, wchar_t const* rootPath)
#endif
{

#if defined(MOYU_PLATFORM_WINDOWS)
    if (instance)
    {
    }
#endif

    Moyu::Log().Info("RuntimeMain Call");
    Moyu::ScriptLoadDesc scriptLoadDesc {
        .AssemblyRootPath   = rootPath,
        .AssemblyName       = L"Editor.dll",
        .AssemblyConfigName = L"Editor.runtimeconfig.json",
        .AssemblyType       = L"Editor.Editor, Editor",
    };

    Moyu::ScriptAssembly scriptAssembly(scriptLoadDesc);

    using EntranceFunction = void (*)();

    EntranceFunction entrance = scriptAssembly.GetFunction<EntranceFunction>(
        L"Entrance");

    entrance();

    return 0;
}
