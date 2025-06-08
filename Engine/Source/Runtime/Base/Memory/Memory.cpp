#include "Memory.h"

#include <format>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>

#include "mimalloc.h"

#include "Runtime/Base/MoyuMacro.h"

struct AllocationInfo
{
    uintptr_t            Ptr;
    size_t               Size;
    size_t               Alignment;
    std::source_location Location;
};

static std::map<void*, AllocationInfo> allocations_;
static std::mutex                      mutex_;

namespace Moyu
{
    void* TrackAllocation(size_t size, size_t alignment,
                          const std::source_location& loc)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        void* reportedAddress = mi_malloc_aligned(size, alignment);
        MOYU_ASSERT(reportedAddress != nullptr);
        allocations_[reportedAddress] = AllocationInfo {
            reinterpret_cast<uintptr_t>(reportedAddress), size, alignment, loc};
        return reportedAddress;
    }

    void MOYU_BASE_CALL TrackDeallocation(void*  reportedAddress,
                                          size_t alignment)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto                        it = allocations_.find(reportedAddress);
        MOYU_ASSERT(it != allocations_.end());
        allocations_.erase(it);
        mi_free_aligned(reportedAddress, alignment);
    }

    void ReportLeaks()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (allocations_.empty())
        {
            std::cout << "No memory leaks detected.\n";
        }
        else
        {
            std::cerr << std::format(
                "Memory leak detected: {} allocation(s) not freed:\n",
                allocations_.size());

            for (const auto& [ptr, info] : allocations_)
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
