#pragma once

#include <cstdint>
#include <memory>
#include <source_location>

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

        inline size_t AlignUp(size_t size, size_t alignment)
        {
            return (size + alignment - 1) & (~(alignment - 1));
        }

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

    [[nodiscard]] MOYU_BASE_API void* MOYU_BASE_CALL TrackAllocation(
        size_t size, size_t alignment,
        const std::source_location& loc = std::source_location::current());

    MOYU_BASE_API void MOYU_BASE_CALL TrackDeallocation(void*  reportedAddress,
                                                        size_t alignment);

    MOYU_BASE_API void MOYU_BASE_CALL ReportLeaks();

    template<typename T>
    [[nodiscard]] inline T*
    Malloc(size_t                      size = sizeof(T),
           const std::source_location& loc  = std::source_location::current())
    {
        size_t alignment = alignof(T);
        alignment     = alignment < sizeof(void*) ? sizeof(void*) : alignment;
        void* pMemory = TrackAllocation(size, alignment, loc);
        return static_cast<T*>(pMemory);
    }

    template<typename T, typename... Args>
    [[nodiscard]] inline T*
    New(Args&&... args,
        const std::source_location& loc = std::source_location::current())
    {
        size_t alignment = alignof(T);
        alignment     = alignment < sizeof(void*) ? sizeof(void*) : alignment;
        void* pMemory = TrackAllocation(sizeof(T), alignment, loc);
        return new (pMemory) T(std::forward<Args>(args)...);
    }

    template<typename T>
    inline void Delete(T*& pType)
    {
        if (pType != nullptr)
        {
            size_t alignment = alignof(T);
            alignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;

            pType->~T();
            TrackDeallocation(pType, alignment);
            pType = nullptr;
        }
    }

    template<typename T>
    inline void Free(T*& pType)
    {
        if (pType != nullptr)
        {
            size_t alignment = alignof(T);
            alignment = alignment < sizeof(void*) ? sizeof(void*) : alignment;

            TrackDeallocation(pType, alignment);
            pType = nullptr;
        }
    }

} // namespace Moyu
