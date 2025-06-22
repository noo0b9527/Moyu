// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

#include "Module/Script.h"

#include <assert.h>

#include <iostream>
#include <string>

#include "Base/Config.h"
#include "Base/DynamicLibrary.h"
#include "Base/String.h"

#include "dotnethost/coreclr_delegates.h"
#include "dotnethost/hostfxr.h"
#include "dotnethost/nethost.h"

#if defined(MOYU_PLATFORM_WINDOWS)
#include <windows.h>
#endif

using HostfxrInitForCommandLineFn =
    hostfxr_initialize_for_dotnet_command_line_fn;
using HostfxrInitForRuntimeConfigFn = hostfxr_initialize_for_runtime_config_fn;
using HostfxrGetRuntimeDelegateFn   = hostfxr_get_runtime_delegate_fn;
using HostfxrRunAppFn               = hostfxr_run_app_fn;
using HostfxrCloseFn                = hostfxr_close_fn;
using HostfxrParameters             = get_hostfxr_parameters;
using LoadAssemblyAndGetFunctionPointer =
    load_assembly_and_get_function_pointer_fn;

static HostfxrInitForCommandLineFn       s_InitForCmdLineFptr = nullptr;
static HostfxrInitForRuntimeConfigFn     s_InitForConfigFptr  = nullptr;
static HostfxrGetRuntimeDelegateFn       s_GetDelegateFptr    = nullptr;
static HostfxrRunAppFn                   s_RunAppFptr         = nullptr;
static HostfxrCloseFn                    s_CloseFptr          = nullptr;
static LoadAssemblyAndGetFunctionPointer s_AssemblyFunctionPointerGetters =
    nullptr;

// Using the nethost library, discover the location of hostfxr and get
// exports
static bool LoadHostfxr(char_t const* assemblyPath)
{
    using namespace Moyu;
    HostfxrParameters params {sizeof(HostfxrParameters), assemblyPath, nullptr};

    // Pre-allocate a large buffer for the path to hostfxr
    char_t buffer[MAX_PATH] {};
    size_t bufferSize = sizeof(buffer) / sizeof(char_t);
    int    rc         = get_hostfxr_path(buffer, &bufferSize, &params);
    if (rc != 0)
        return false;

    // Load hostfxr and get desired exports
    // NOTE: The .NET Runtime does not support unloading any of its native
    // libraries. Running dlclose/FreeLibrary on any .NET libraries produces
    // undefined behavior.
    DynamicLibrary library(buffer);
    s_InitForCmdLineFptr = library.GetFunction<HostfxrInitForCommandLineFn>(
        "hostfxr_initialize_for_dotnet_command_line");

    s_InitForConfigFptr = library.GetFunction<HostfxrInitForRuntimeConfigFn>(
        "hostfxr_initialize_for_runtime_config");

    s_GetDelegateFptr = library.GetFunction<HostfxrGetRuntimeDelegateFn>(
        "hostfxr_get_runtime_delegate");

    s_RunAppFptr = library.GetFunction<HostfxrRunAppFn>("hostfxr_run_app");

    s_CloseFptr = library.GetFunction<HostfxrCloseFn>("hostfxr_close");

    return (s_InitForConfigFptr && s_GetDelegateFptr && s_CloseFptr);
}

// Load and initialize .NET Core and get desired function pointer for scenario
static load_assembly_and_get_function_pointer_fn
GetDotnetLoadAssembly(char_t const* configPath)
{
    void*          loadAssemblyAndGetFunctionPointer = nullptr;
    hostfxr_handle cxt                               = nullptr;
    int            rc = s_InitForConfigFptr(configPath, nullptr, &cxt);
    if (rc != 0 || cxt == nullptr)
    {
        std::cerr << "Init failed: " << std::hex << std::showbase << rc
                  << std::endl;
        s_CloseFptr(cxt);
        return nullptr;
    }

    rc = s_GetDelegateFptr(cxt, hdt_load_assembly_and_get_function_pointer,
                           &loadAssemblyAndGetFunctionPointer);
    if (rc != 0 || loadAssemblyAndGetFunctionPointer == nullptr)
        std::cerr << "Get delegate failed: " << std::hex << std::showbase << rc
                  << std::endl;
    s_CloseFptr(cxt);

    return static_cast<load_assembly_and_get_function_pointer_fn>(
        loadAssemblyAndGetFunctionPointer);
}

namespace Moyu
{
    ScriptAssembly::ScriptAssembly(ScriptLoadDesc& desc)
    {
        m_assemblyRootPath = desc.AssemblyRootPath;
        m_assemblyType     = desc.AssemblyType;
        m_assemblyName     = desc.AssemblyName;
        //
        // STEP 1: Load HostFxr and get exported hosting functions
        //
        if (!LoadHostfxr(nullptr))
        {
            assert(false && "Failure: load_hostfxr()");
        }

        //
        // STEP 2: Initialize and start the .NET Core runtime
        //
        const WString configath = m_assemblyRootPath + desc.AssemblyConfigName;
        s_AssemblyFunctionPointerGetters =
            GetDotnetLoadAssembly(configath.c_str());
        assert(s_AssemblyFunctionPointerGetters != nullptr &&
               "Failure: GetDotnetLoadAssembly()");
    }

    void* ScriptAssembly::GetFunctionInternal(WString const&        name,
                                              ScriptAssembly const* assembly)
    {
        void* functionPtr {};
        int   rc = s_AssemblyFunctionPointerGetters(
            assembly->GetAssemblyNameWithRootPath().c_str(),
            assembly->GetAssemblyType(), name.c_str(),
            UNMANAGEDCALLERSONLY_METHOD, nullptr,
            reinterpret_cast<void**>(&functionPtr));

        assert(rc == 0 && functionPtr != nullptr &&
               "Failure: LoadAssemblyAndGetFunctionPointer()");

        return functionPtr;
    }

} // namespace Moyu
