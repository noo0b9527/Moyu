#include "Memory.h"

#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>

#include "mimalloc.h"

#include "Runtime/Base/MoyuMacro.h"

namespace Moyu
{
    struct AllocationInfo
    {
        uintptr_t            Ptr;
        size_t               Size;
        size_t               Alignment;
        std::source_location Location;
    };

    static std::map<void*, AllocationInfo> s_Allocations;
    static std::mutex                      s_Mutex;

    void* TrackAllocation(size_t size, size_t alignment,
                          const std::source_location& loc)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        void* reportedAddress = mi_malloc_aligned(size, alignment);
        MOYU_ASSERT(reportedAddress != nullptr);
        s_Allocations[reportedAddress] = AllocationInfo {
            reinterpret_cast<uintptr_t>(reportedAddress), size, alignment, loc};
        return reportedAddress;
    }

    void TrackDeallocation(void* reportedAddress, size_t alignment)
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        auto                        it = s_Allocations.find(reportedAddress);
        MOYU_ASSERT(it != s_Allocations.end());
        s_Allocations.erase(it);
        mi_free_aligned(reportedAddress, alignment);
    }

    void ReportLeaks()
    {
        std::lock_guard<std::mutex> lock(s_Mutex);
        if (s_Allocations.empty())
        {
            std::cout << "No memory leaks detected.\n";
        }
        else
        {
            std::cerr << std::format(
                "Memory leak detected: {} allocation(s) not freed:\n",
                s_Allocations.size());

            for (const auto& [ptr, info] : s_Allocations)
            {
                std::cerr << std::format("Leaked {} bytes (aligned to {}) at "
                                         "{} allocated at {}:{}\n",
                                         info.Size, info.Alignment, ptr,
                                         info.Location.file_name(),
                                         info.Location.line());
            }
        }
    }

} // namespace Moyu
