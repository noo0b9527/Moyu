#pragma once

#include <cstdint>
#include <memory>
#include <source_location>
#include <utility>

#include "Runtime/Base/_Module/API.h"

namespace Moyu
{
    namespace Memory
    {
        inline void MemsetZero(void* ptr, size_t size)
        {
            std::memset(ptr, 0, size);
        }

        template<typename T>
        inline void MemsetZero(T* ptr)
        {
            memset(ptr, 0, sizeof(T));
        }

        inline bool IsAligned(void const* p, size_t n)
        {
            return (reinterpret_cast<uintptr_t>(p) % n) == 0;
        }

        template<typename T>
        inline bool IsAligned(T const* p)
        {
            return (reinterpret_cast<uintptr_t>(p) % alignof(T)) == 0;
        }

        inline size_t CalculatePaddingForAlignment(uintptr_t addressOffset,
                                                   size_t    requiredAlignment)
        {
            return (requiredAlignment - (addressOffset % requiredAlignment)) %
                   requiredAlignment;
        }

        inline size_t CalculatePaddingForAlignment(void*  address,
                                                   size_t requiredAlignment)
        {
            return CalculatePaddingForAlignment(
                reinterpret_cast<uintptr_t>(address), requiredAlignment);
        }

        MOYU_BASE_API size_t MOYU_BASE_CALL GetTotalRequestedMemory();
        MOYU_BASE_API size_t MOYU_BASE_CALL GetTotalAllocatedMemory();
        MOYU_BASE_API void                  DumpMemoryReport();

    } // namespace Memory

    enum class AllocationType : uint8_t
    {
        kAllocUnkown      = 0,
        kAllocNew         = 1,
        kAllocNewArray    = 2,
        kAllocMalloc      = 3,
        kAllocCalloc      = 4,
        kAllocRealloc     = 5,
        kAllocDelete      = 6,
        kAllocDeleteArray = 7,
        kAllocFree        = 8,
    };

    [[nodiscard]] MOYU_BASE_API void* MOYU_BASE_CALL
    Allocator(char const* sourceFile, uint32_t const sourceLine,
              char const* sourceFunc, AllocationType const allocationType,
              size_t const reportedSize, size_t const alignment);

    [[nodiscard]] MOYU_BASE_API void* MOYU_BASE_CALL
    Reallocator(char const* sourceFile, uint32_t const sourceLine,
                char const* sourceFunc, AllocationType const reallocationType,
                size_t const reportedSize, void* reportedAddress);

    MOYU_BASE_API void MOYU_BASE_CALL
    Deallocator(char const* sourceFile, uint32_t const sourceLine,
                char const* sourceFunc, AllocationType const deallocationType,
                const void* reportedAddress);

    template<typename T>
    [[nodiscard]] inline T*
    Malloc(size_t                      size = sizeof(T),
           const std::source_location& loc  = std::source_location::current())
    {
        size_t alignment = alignof(T);
        alignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;
        void* pMemory =
            Allocator(loc.file_name(), loc.line(), loc.function_name(),
                      AllocationType::kAllocMalloc, size, alignment);
        return static_cast<T*>(pMemory);
    }

    template<typename T>
    [[nodiscard]] inline T*
    Realloc(T* pType, size_t reportedSize,
            const std::source_location& loc = std::source_location::current())
    {
        size_t alignment = alignof(T);
        alignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;
        void* pMemory =
            Reallocator(loc.file_name(), loc.line(), loc.function_name(),
                        AllocationType::kAllocRealloc, reportedSize, pType);
        return static_cast<T*>(pMemory);
    }

    template<typename T, typename... Args>
    [[nodiscard]] inline T*
    New(Args&&... args,
        const std::source_location& loc = std::source_location::current())
    {
        size_t alignment = alignof(T);
        alignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;
        void* pMemory =
            Allocator(loc.file_name(), loc.line(), loc.function_name(),
                      AllocationType::kAllocNew, sizeof(T), alignment);
        return new (pMemory) T(std::forward<Args>(args)...);
    }

    template<typename T>
    inline void
    Delete(T*&                         pType,
           const std::source_location& loc = std::source_location::current())
    {
        if (pType != nullptr)
        {
            pType->~T();
            Deallocator(loc.file_name(), loc.line(), loc.function_name(),
                        AllocationType::kAllocDelete, pType);
            pType = nullptr;
        }
    }

    template<typename T>
    inline void
    Free(T*&                         pType,
         const std::source_location& loc = std::source_location::current())
    {
        if (pType != nullptr)
        {
            Deallocator(loc.file_name(), loc.line(), loc.function_name(),
                        AllocationType::kAllocFree, pType);
            pType = nullptr;
        }
    }

} // namespace Moyu
