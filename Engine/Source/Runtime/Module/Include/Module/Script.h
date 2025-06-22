#pragma once

#include <assert.h>
#include <stdint.h>

#include "Base/String.h"

namespace Moyu
{
    struct ScriptLoadDesc
    {
        WString AssemblyRootPath;
        WString AssemblyName;
        WString AssemblyConfigName;
        WString AssemblyType;
        WString AssemblyVersion;
    };

    class ScriptAssembly
    {
    public:
        explicit ScriptAssembly(ScriptLoadDesc& desc);
        ~ScriptAssembly() = default;

        ScriptAssembly(ScriptAssembly const&)            = delete;
        ScriptAssembly& operator=(ScriptAssembly const&) = delete;

        ScriptAssembly(ScriptAssembly&&)                  = delete;
        ScriptAssembly& operator=(ScriptAssembly&& other) = delete;

        template<typename FuncPtr>
        FuncPtr GetFunction(WString const& name) const
        {
            static_assert(std::is_pointer_v<FuncPtr>,
                          "FuncPtr must be a function pointer");

            FuncPtr functionPtr {};
            functionPtr = static_cast<FuncPtr>(GetFunctionInternal(name, this));

            assert(functionPtr != nullptr &&
                   "Failure: LoadAssemblyAndGetFunctionPointer()");

            return functionPtr;
        }

        inline wchar const* GetAssemblyRootPath() const
        {
            return m_assemblyRootPath.c_str();
        }

        inline wchar const* GetAssemblyType() const
        {
            return m_assemblyType.c_str();
        }

        inline wchar const* GetAssemblyName() const
        {
            return m_assemblyName.c_str();
        }

        inline std::wstring GetAssemblyNameWithRootPath() const
        {
            return m_assemblyRootPath + m_assemblyName;
        }

    private:
        static void* GetFunctionInternal(WString const&        name,
                                         ScriptAssembly const* assembly);

        WString m_assemblyRootPath;
        WString m_assemblyType;
        WString m_assemblyName;

        void* m_handle = nullptr; // Handle to the loaded script assembly
    };

} // namespace Moyu
