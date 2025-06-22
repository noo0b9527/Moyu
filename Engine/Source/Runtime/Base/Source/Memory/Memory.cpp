#include "Base/Memory.h"

#include <assert.h>

#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>

#include "mimalloc.h"


struct AllocationInfo
{
    uintptr_t            Ptr;
    size_t               Size;
    size_t               Alignment;
    std::source_location Location;
};

static std::map<void*, AllocationInfo> kAllocations;
static std::mutex                      kAmutex;

namespace Moyu
{
    void* TrackAllocation(size_t size, size_t alignment,
                          const std::source_location& loc)
    {
        std::lock_guard<std::mutex> lock(kAmutex);
        void* reportedAddress = mi_malloc_aligned(size, alignment);
        assert(reportedAddress != nullptr && "Malloc memory fatal");
        kAllocations[reportedAddress] = AllocationInfo {
            reinterpret_cast<uintptr_t>(reportedAddress), size, alignment, loc};
        return reportedAddress;
    }

    void TrackDeallocation(void* reportedAddress, size_t alignment)
    {
        std::lock_guard<std::mutex> lock(kAmutex);
        auto                        it = kAllocations.find(reportedAddress);
        assert(it != kAllocations.end() && "Find reportedAddress fatal");
        kAllocations.erase(it);
        mi_free_aligned(reportedAddress, alignment);
    }

    void ReportLeaks()
    {
        std::lock_guard<std::mutex> lock(kAmutex);
        if (kAllocations.empty())
        {
            std::cout << "No memory leaks detected.\n";
        }
        else
        {
            std::cerr << std::format(
                "Memory leak detected: {} allocation(s) not freed:\n",
                kAllocations.size());

            for (const auto& [ptr, info] : kAllocations)
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
