#pragma once

#include <assert.h>
#include <iostream>

#include "Base/String.h"

namespace Moyu
{
    class DynamicLibrary
    {
    public:
        explicit DynamicLibrary(WString& libraryPath);
        explicit DynamicLibrary(wchar const* libraryPath);

        DynamicLibrary(const DynamicLibrary&)            = delete;
        DynamicLibrary& operator=(const DynamicLibrary&) = delete;

        DynamicLibrary(DynamicLibrary&&)            = delete;
        DynamicLibrary& operator=(DynamicLibrary&&) = delete;

        ~DynamicLibrary() = default;

        template<typename FuncPtr>
        FuncPtr GetFunction(char const* name) const
        {
            static_assert(std::is_pointer_v<FuncPtr>,
                          "FuncPtr must be a function pointer");
            FuncPtr funcPtr {};

            funcPtr = static_cast<FuncPtr>(GetExport(m_handle, name));
            assert(funcPtr != nullptr && "Failed to get export function");

            return funcPtr;
        }

        inline bool IsLoaded() const
        {
            return m_handle != nullptr;
        }

    private:
        static void  FreeLibrary(void* handle);
        static void* GetExport(void* h, char const* name);

        void* m_handle = nullptr;
    };

} // namespace Moyu
