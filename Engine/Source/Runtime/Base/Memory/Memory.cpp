#include "Runtime/Base/Memory/Memory.h"

#if defined(_WIN32)
#include <windows.h>
#endif

#include <array>
#include <deque>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Runtime/Base/MoyuMacro.h"

namespace Moyu
{
    namespace
    {
#ifdef STRESS_TEST
        bool               s_RandomWipe           = true;
        bool               s_AlwaysValidateAll    = true;
        bool               s_AlwaysLogAll         = true;
        bool               s_AlwaysWipeAll        = true;
        bool               s_CleanupLogOnFirstRun = true;
        constexpr uint32_t kPaddingSize = 1024u; // An extra 8K per allocation!
        constexpr uint32_t kHashBits    = 12u;
#else
        bool               s_RandomWipe           = false;
        bool               s_AlwaysValidateAll    = false;
        bool               s_AlwaysLogAll         = false;
        bool               s_AlwaysWipeAll        = true;
        bool               s_CleanupLogOnFirstRun = true;
        constexpr uint32_t kPaddingSize           = 4u;
        constexpr uint32_t kHashBits              = 12u;
#endif

        struct AllocUnit
        {
            size_t         ActualSize;
            size_t         ReportedSize;
            size_t         Alignment;
            size_t         Offset;
            void*          ActualAddress;
            void*          ReportedAddress;
            char           SourceFile[140];
            char           SourceFunc[140];
            uint32_t       SourceLine;
            AllocationType AllocationType;
            bool           BreakOnDealloc;
            bool           BreakOnRealloc;
            uint32_t       AllocationNumber;
            AllocUnit*     Next;
            AllocUnit*     Prev;
            // size_t         Index;
        };

        struct MStats
        {
            uint64_t TotalReportedMemory;
            uint64_t TotalActualMemory;
            uint64_t PeakReportedMemory;
            uint64_t PeakActualMemory;
            uint64_t AccumulatedReportedMemory;
            uint64_t AccumulatedActualMemory;
            uint64_t AccumulatedAllocUnitCount;
            uint64_t TotalAllocUnitCount;
            uint64_t PeakAllocUnitCount;
        };

        constexpr uint64_t                        kHashSize = 1u << kHashBits;
        constexpr std::array<std::string_view, 9> kAllocationTypeNames = {
            "Unknown", "new",    "new[]",    "malloc", "calloc",
            "realloc", "delete", "delete[]", "free"};

        std::mutex                            s_Mutex {};
        std::unordered_map<void*, AllocUnit*> s_HashTable {};

        AllocUnit*  s_pReservoir {};
        AllocUnit** s_ppReservoirBuffer {};

        // std::deque<AllocUnit> s_AllocUnits;
        // std::vector<size_t>   s_FreeIndices;

        MStats       s_Stats {};
        uint32_t     s_CurrentAllocationCount {};
        uint32_t     s_BreakOnAllocationCount {};
        uint32_t     s_ReservoirBufferSize = 0;
        std::mt19937 s_RandomEngine {std::random_device {}()};
        std::uniform_int_distribution<uint32_t>
            s_RandomDistribution(0, 0xFFFFFFFF);

        // ---------------------------------------------------------------------------------------------------------------------------------
        // -DOC- Get to know these values. They represent the values that will
        // be used to fill unused and deallocated RAM.
        // ---------------------------------------------------------------------------------------------------------------------------------
        // Fill pattern for bytes preceeding allocated blocks
        constexpr uint32_t kPrefixPattern = 0xbaadf00d;
        // Fill pattern for bytes following allocated blocks
        constexpr uint32_t kPostfixPattern = 0xdeadc0de;
        // Fill pattern for freshly allocated blocks
        constexpr uint32_t kUnusedPattern = 0xfeedface;
        // Fill pattern for deallocated blocks
        constexpr uint32_t kReleasedPattern = 0xdeadbeef;

        // DumpUsed???
        // #if defined(_WIN32)
        //     HANDLE s_ProcessHandle {};
        // #endif

    } // namespace

    static size_t CalculateActualSize(const size_t reportedSize,
                                      size_t       alignment)
    {
        constexpr size_t kPaddingBytes = kPaddingSize * sizeof(uint32_t);
        return reportedSize + 2 * kPaddingBytes + alignment;
    }

    static void* CalculateReportedAddress(const void* actualAddress,
                                          size_t alignment, size_t& outOffset)
    {
        // We allow this
        if (actualAddress == nullptr)
            return nullptr;

        // Just account for the padding
        const uint8_t* address = static_cast<const uint8_t*>(actualAddress) +
                                 sizeof(uint32_t) * kPaddingSize;

        std::uintptr_t offset =
            reinterpret_cast<std::uintptr_t>(address) % alignment;

        if (offset != 0)
        {
            // Align the address
            address += (alignment - offset);
        }

        outOffset = offset;
        return const_cast<uint8_t*>(address);
    }

    static void WipeWithPattern(AllocUnit* allocUnit, uint32_t pattern,
                                const uint32_t originalReportedSize = 0)
    {
        // For a serious test run, we use wipes of random a random value.
        // However, if this causes a crash, we don't want it to crash in a
        // differnt place each time, so we specifically DO NOT call srand. If,
        // by chance your program calls srand(), you may wish to disable that
        // when running with a random wipe test. This will make any crashes more
        // consistent so they can be tracked down easier.

        if (s_RandomWipe)
        {
            // Use C++11 random library instead of rand()
            pattern = s_RandomDistribution(s_RandomEngine);
        }

        // -DOC- We should wipe with 0's if we're not in debug mode, so we can
        // help hide bugs if possible when we release the product. So uncomment
        // the following line for releases.
        //
        // Note that the "alwaysWipeAll" should be turned on for this to have
        // effect, otherwise it won't do much good. But we'll leave it this way
        // (as an option) because this does slow things down.
        //	pattern = 0;

        // This part of the operation is optional

        if (s_AlwaysWipeAll && allocUnit->ReportedSize > originalReportedSize)
        {
            // Fill the bulk

            uint32_t* lptr = reinterpret_cast<uint32_t*>(
                (reinterpret_cast<char*>(allocUnit->ReportedAddress)) +
                originalReportedSize);

            int length = static_cast<int>(allocUnit->ReportedSize -
                                          originalReportedSize);
            int i;
            for (i = 0; i < (length >> 2); i++, lptr++)
            {
                *lptr = pattern;
            }

            // Fill the remainder

            unsigned int shiftCount = 0;
            char*        cptr       = reinterpret_cast<char*>(lptr);
            for (i = 0; i < (length & 0x3); i++, cptr++, shiftCount += 8)
            {
                *cptr = static_cast<char>((pattern & (0xff << shiftCount)) >>
                                          shiftCount);
            }
        }

        // Write in the prefix/postfix bytes

        // Calculate the correct start addresses for pre and post patterns
        // relative to allocUnit->reportedAddress, since it may have been offset
        // due to alignment requirements
        uint8_t* pre = static_cast<uint8_t*>(allocUnit->ReportedAddress) -
                       kPaddingSize * sizeof(uint32_t);
        uint8_t* post = static_cast<uint8_t*>(allocUnit->ReportedAddress) +
                        allocUnit->ReportedSize;

        const size_t paddingBytes = kPaddingSize * sizeof(uint32_t);

        for (size_t i = 0; i < paddingBytes; i++, pre++, post++)
        {
            *pre  = (kPrefixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
            *post = (kPostfixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
        }
    }

    static bool ValidateAllocUnit(const AllocUnit* allocUnit)
    {
        // Make sure the padding is untouched

        uint8_t* pre  = (static_cast<uint8_t*>(allocUnit->ReportedAddress) -
                        kPaddingSize * sizeof(uint32_t));
        uint8_t* post = (static_cast<uint8_t*>(allocUnit->ReportedAddress) +
                         allocUnit->ReportedSize);

        bool             errorFlag    = false;
        constexpr size_t paddingBytes = kPaddingSize * sizeof(uint32_t);

        for (size_t i = 0; i < paddingBytes; i++, pre++, post++)
        {
            const uint8_t expectedPrefixByte =
                (kPrefixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
            if (*pre != expectedPrefixByte)
            {
                // Log("[!] A memory allocation unit was corrupt because of an "
                //     "underrun:");
                // mmgrDumpAllocUnit(allocUnit, "  ");
                errorFlag = true;
            }

            // If you hit this assert, then you should know that this allocation
            // unit has been damaged. Something (possibly the owner?) has
            // underrun the allocation unit (modified a few bytes prior to the
            // start). You can interrogate the variable 'allocUnit' to see
            // statistics and information about this damaged allocation unit.
            MOYU_ASSERT(*pre == expectedPrefixByte);

            const uint8_t expectedPostfixByte =
                (kPostfixPattern >> ((i % sizeof(uint32_t)) * 8)) & 0xFF;
            if (*post != expectedPostfixByte)
            {
                // Log("[!] A memory allocation unit was corrupt because of an "
                //     "overrun:");
                // mmgrDumpAllocUnit(allocUnit, "  ");
                errorFlag = true;
            }

            // If you hit this assert, then you should know that this allocation
            // unit has been damaged. Something (possibly the owner?) has
            // overrun the allocation unit (modified a few bytes after the end).
            // You can interrogate the variable 'allocUnit' to see statistics
            // and information about this damaged allocation unit.
            MOYU_ASSERT(*post == expectedPostfixByte);
        }

        // Return the error status (we invert it, because a return of 'false'
        // means error)

        return !errorFlag;
    }

    static bool ValidateAllAllocUnits()
    {
        // Just go through each allocation unit in the hash table and count the
        // ones that have errors

        unsigned int errors     = 0;
        unsigned int allocCount = 0;

        for (const auto& [reportedAddress, allocUnit] : s_HashTable)
        {
            allocCount++;
            if (!ValidateAllocUnit(allocUnit))
                errors++;
        }

        // Test for hash-table correctness

        if (allocCount != s_Stats.TotalAllocUnitCount)
        {
            // Log("[!] Memory tracking hash table corrupt!");
            errors++;
        }

        // If you hit this assert, then the internal memory (hash table) used by
        // this memory tracking software is damaged! The best way to track this
        // down is to use the alwaysLogAll flag in conjunction with STRESS_TEST
        // macro to narrow in on the offending code. After running the
        // application with these settings (and hitting this assert again),
        // interrogate the memory.log file to find the previous successful
        // operation. The corruption will have occurred between that point and
        // this assertion.
        MOYU_ASSERT(allocCount == s_Stats.TotalAllocUnitCount);

        // If you hit this assert, then you've probably already been notified
        // that there was a problem with a allocation unit in a prior call to
        // validateAllocUnit(), but this assert is here just to make sure you
        // know about it. :)
        MOYU_ASSERT(errors == 0);

        // Log any errors

        if (errors)
        {
            // Log("[!] While validting all allocation units, %d allocation "
            //     "unit(s) were found to have problems",
            //     errors);
        }
        // Return the error status

        return errors != 0;
    }

    static AllocUnit* FindAllocUnit(const void* reportedAddress)
    {
        // Just in case...
        MOYU_ASSERT(reportedAddress != nullptr);

        auto it = s_HashTable.find(const_cast<void*>(reportedAddress));
        return (it != s_HashTable.end()) ? it->second : nullptr;
    }

    static bool IsValidDeallocationPair(AllocationType dealloc,
                                        AllocationType alloc)
    {
        return (dealloc == AllocationType::kAllocDelete &&
                alloc == AllocationType::kAllocNew) ||
               (dealloc == AllocationType::kAllocDeleteArray &&
                alloc == AllocationType::kAllocNewArray) ||
               (dealloc == AllocationType::kAllocFree &&
                (alloc == AllocationType::kAllocMalloc ||
                 alloc == AllocationType::kAllocCalloc ||
                 alloc == AllocationType::kAllocRealloc)) ||
               (dealloc == AllocationType::kAllocUnkown);
    }

    // static AllocUnit* AllocateAllocUnit()
    //{
    //     size_t idx;
    //     if (!s_FreeIndices.empty())
    //     {
    //         idx = s_FreeIndices.back();
    //         s_FreeIndices.pop_back();
    //     }
    //     else
    //     {
    //         s_AllocUnits.emplace_back();
    //         idx = s_AllocUnits.size() - 1;
    //     }
    //     AllocUnit* au = &s_AllocUnits[idx];
    //     au->Index     = idx;
    //     return au;
    // }

    // static void FreeAllocUnit(AllocUnit* unit)
    //{
    //     s_FreeIndices.push_back(unit->Index);
    // }

    void* Allocator(char const* sourceFile, uint32_t const sourceLine,
                    char const* sourceFunc, AllocationType const allocationType,
                    size_t const reportedSize, size_t const alignment)
    {
        constexpr size_t                  reservoirSize = 256;
        std::lock_guard<std::mutex> const lock(s_Mutex);
        s_CurrentAllocationCount++;

        MOYU_ASSERT(s_CurrentAllocationCount != s_BreakOnAllocationCount);

        if (s_pReservoir == nullptr)
        {
            // Todo Use Other malloc implementations??
            void* pTemp = nullptr;
#if defined(_WIN32)
            pTemp = _aligned_malloc(sizeof(AllocUnit) * reservoirSize,
                                    alignof(AllocUnit));
#endif
            s_pReservoir = static_cast<AllocUnit*>(pTemp);
            MOYU_ASSERT(s_pReservoir != nullptr);

            if (s_pReservoir == nullptr)
            {
                // Todo: Give More Infomation
                MOYU_ASSERT(false);
            }

            memset(s_pReservoir, 0, sizeof(AllocUnit) * reservoirSize);
            for (size_t i = 0; i < reservoirSize - 1; i++)
            {
                s_pReservoir[i].Next = &s_pReservoir[i + 1];
            }

            size_t reallocSize =
                (static_cast<size_t>(s_ReservoirBufferSize) + 1) *
                sizeof(AllocUnit*);

            void* ppTemp = nullptr;
#if defined(_WIN32)
            ppTemp = _aligned_realloc(s_ppReservoirBuffer, reallocSize,
                                      alignof(AllocUnit*));

#endif
            AllocUnit** temp = static_cast<AllocUnit**>(ppTemp);
            MOYU_ASSERT(temp != nullptr);

            if (temp)
            {
                s_ppReservoirBuffer                          = temp;
                s_ppReservoirBuffer[s_ReservoirBufferSize++] = s_pReservoir;
            }
        }

        size_t     outOffset {};
        AllocUnit* au = s_pReservoir;
        s_pReservoir  = au->Next;

        // AllocUnit* au = AllocateAllocUnit();
        memset(au, 0, sizeof(AllocUnit));
        au->ActualSize = CalculateActualSize(reportedSize, alignment);
#if defined(_WIN32)
        au->ActualAddress = _aligned_malloc(au->ActualSize, alignment);
#endif
        au->ReportedSize = reportedSize;
        au->ReportedAddress =
            CalculateReportedAddress(au->ActualAddress, alignment, outOffset);
        au->Offset = outOffset;

        au->AllocationType   = allocationType;
        au->AllocationNumber = s_CurrentAllocationCount;
        au->Alignment        = alignment;
        au->SourceLine       = sourceLine;
        if (sourceFunc != nullptr)
        {
            strncpy(au->SourceFunc, sourceFunc, sizeof(au->SourceFunc) - 1);
            au->SourceFunc[sizeof(au->SourceFunc) - 1] = '\0';
        }
        else
        {
            strcpy(au->SourceFunc, "??");
        }

        if (sourceFile != nullptr)
        {
            strncpy(au->SourceFile, sourceFile, sizeof(au->SourceFile) - 1);
            au->SourceFile[sizeof(au->SourceFile) - 1] = '\0';
        }
        else
        {
            strcpy(au->SourceFile, "??");
        }

        MOYU_ASSERT(au->ActualAddress != nullptr);

        if (au->ActualAddress == nullptr)
        {
            MOYU_ASSERT(false);
        }

        MOYU_ASSERT(allocationType != AllocationType::kAllocUnkown);

        s_HashTable[au->ReportedAddress] = au;

        s_Stats.TotalReportedMemory += static_cast<uint64_t>(au->ReportedSize);
        s_Stats.TotalActualMemory += static_cast<uint64_t>(au->ActualSize);
        s_Stats.TotalAllocUnitCount++;
        if (s_Stats.TotalReportedMemory > s_Stats.PeakReportedMemory)
        {
            s_Stats.PeakReportedMemory = s_Stats.TotalReportedMemory;
        }
        if (s_Stats.TotalActualMemory > s_Stats.PeakActualMemory)
        {
            s_Stats.PeakActualMemory = s_Stats.TotalActualMemory;
        }
        if (s_Stats.TotalAllocUnitCount > s_Stats.PeakAllocUnitCount)
        {
            s_Stats.PeakAllocUnitCount = s_Stats.TotalAllocUnitCount;
        }
        s_Stats.AccumulatedReportedMemory +=
            static_cast<unsigned int>(au->ReportedSize);
        s_Stats.AccumulatedActualMemory +=
            static_cast<unsigned int>(au->ActualSize);
        s_Stats.AccumulatedAllocUnitCount++;

        WipeWithPattern(au, kUnusedPattern);

        if (allocationType == AllocationType::kAllocCalloc)
        {
            memset(au->ReportedAddress, 0, au->ReportedSize);
        }

        if (s_AlwaysValidateAll)
        {
            ValidateAllAllocUnits();
        }

        if (s_AlwaysLogAll)
        {
        }

        MOYU_ASSERT(Memory::IsAligned(au->ReportedAddress, alignment));

        return au->ReportedAddress;
    }

    void* Reallocator(char const* sourceFile, uint32_t const sourceLine,
                      char const*          sourceFunc,
                      AllocationType const reallocationType,
                      size_t const reportedSize, void* reportedAddress)
    {

        return nullptr;
    }

    void Deallocator(char const* sourceFile, uint32_t const sourceLine,
                     char const*          sourceFunc,
                     AllocationType const deallocationType,
                     const void*          reportedAddress)
    {
        std::lock_guard<std::mutex> const lock(s_Mutex);

        if (reportedAddress != nullptr)
        {
            AllocUnit* au = FindAllocUnit(reportedAddress);
            MOYU_ASSERT(au != nullptr);

            // If you hit this assert, you tried to deallocate RAM that wasn't
            // allocated by this memory manager.
            if (au == nullptr)
            {
                // Log("[!] Attempt to deallocate memory that was never
                // allocated "
                //     "by this memory manager");
                MOYU_ASSERT(false);
            }

            // If you hit this assert, then the allocation unit that is about to
            // be deallocated is damaged. But you probably already know that
            // from a previous assert you should have seen in
            // validateAllocUnit() :)
            MOYU_ASSERT(ValidateAllocUnit(au));

            // If you hit this assert, then this deallocation was made from a
            // source that isn't setup to use this memory
            // tracking software, use the stack frame to locate the source and
            // include our H file.
            MOYU_ASSERT(deallocationType != AllocationType::kAllocUnkown);

            // If you hit this assert, you were trying to deallocate RAM that
            // was not allocated in a way that is compatible with
            // the deallocation method requested. In other words, you have a
            // allocation/deallocation mismatch.
            MOYU_ASSERT(
                IsValidDeallocationPair(deallocationType, au->AllocationType));

            // If you hit this assert, then the "break on dealloc" flag for this
            // allocation unit is set. Interrogate the 'au'
            // variable to determine information about this allocation unit.
            MOYU_ASSERT(au->BreakOnDealloc == false);

            // Wipe the deallocated RAM with a new pattern. This doen't actually
            // do us much good in debug mode under WIN32,
            // because Microsoft's memory debugging & tracking utilities will
            // wipe it right after we do. Oh well.
            WipeWithPattern(au, kReleasedPattern, 0);

            s_HashTable.erase(au->ReportedAddress);

#if defined(_WIN32)
            _aligned_free(au->ActualAddress);
#endif

            // Remove this allocation from our stats
            s_Stats.TotalReportedMemory -=
                static_cast<uint64_t>(au->ReportedSize);
            s_Stats.TotalActualMemory -= static_cast<uint64_t>(au->ActualSize);
            s_Stats.TotalAllocUnitCount--;

            memset(au, 0, sizeof(AllocUnit));
            // FreeAllocUnit(au);
            au->Next     = s_pReservoir;
            s_pReservoir = au;

            if (s_AlwaysValidateAll)
            {
                ValidateAllAllocUnits();
            }
        }
    }

    namespace Memory
    {
        static const char* insertCommas(size_t value)
        {
            static char str[30];
            memset(str, 0, sizeof(str));

            sprintf(str, "%zu", value);
            if (strlen(str) > 3)
            {
                memmove(&str[strlen(str) - 3], &str[strlen(str) - 4], 4);
                str[strlen(str) - 4] = ',';
            }
            if (strlen(str) > 7)
            {
                memmove(&str[strlen(str) - 7], &str[strlen(str) - 8], 8);
                str[strlen(str) - 8] = ',';
            }
            if (strlen(str) > 11)
            {
                memmove(&str[strlen(str) - 11], &str[strlen(str) - 12], 12);
                str[strlen(str) - 12] = ',';
            }

            return str;
        }

        static const char* memorySizeString(size_t size)
        {
            static char str[90];
            if (size > (1024 * 1024))
                sprintf(str, "%10s (%7.2fM)", insertCommas(size),
                        static_cast<float>(size) / (1024.0f * 1024.0f));
            else if (size > 1024)
                sprintf(str, "%10s (%7.2fK)", insertCommas(size),
                        static_cast<float>(size) / 1024.0f);
            else
                sprintf(str, "%10s bytes     ", insertCommas(size));
            return str;
        }

        static uint32_t CalcUnused(const AllocUnit* allocUnit)
        {
            const uint32_t* ptr =
                reinterpret_cast<const uint32_t*>(allocUnit->ReportedAddress);
            unsigned int count = 0;

            for (unsigned int i = 0; i < allocUnit->ReportedSize;
                 i += sizeof(uint32_t), ptr++)
            {
                if (*ptr == kUnusedPattern)
                    count += sizeof(uint32_t);
            }

            return count;
        }

        static uint32_t CalcAllUnused()
        {
            // Just go through each allocation unit in the hash table and count
            // the unused RAM

            uint32_t total = 0;
            for (const auto& [reportedAddress, allocUnit] : s_HashTable)
            {
                total += CalcUnused(allocUnit);
            }

            return total;
        }

        size_t GetTotalRequestedMemory() { return s_Stats.PeakReportedMemory; }

        size_t GetTotalAllocatedMemory() { return s_Stats.PeakActualMemory; }

        static const char* sourceFileStripper(const char* sourceFile)
        {
            const char* ptr = strrchr(sourceFile, '\\');
            if (ptr)
                return ptr + 1;
            ptr = strrchr(sourceFile, '/');
            if (ptr)
                return ptr + 1;
            return sourceFile;
        }

        static const char* ownerString(const char*        sourceFile,
                                       const unsigned int sourceLine,
                                       const char*        sourceFunc)
        {
            static char str[90];
            memset(str, 0, sizeof(str));
            sprintf(str, "%s(%05d)::%s", sourceFileStripper(sourceFile),
                    sourceLine, sourceFunc);
            return str;
        }

        void DumpMemoryReport()
        {

            std::cout << " ----------------------------------------------------"
                         "-----------------------------------------------------"
                         "------------------------- \r\n";
            std::cout << "|                                                    "
                         "       T O T A L S                                   "
                         "                         |\r\n";
            std::cout << " ----------------------------------------------------"
                         "-----------------------------------------------------"
                         "------------------------- \r\n";

            std::cout << "              Allocation unit count: "
                      << insertCommas(s_Stats.TotalAllocUnitCount) << "\r\n";

            std::cout << "            Reported to application: "
                      << memorySizeString(s_Stats.TotalReportedMemory)
                      << "\r\n";

            std::cout << "         Actual total memory in use: "
                      << memorySizeString(s_Stats.TotalActualMemory) << "\r\n";

            std::cout << "           Memory tracking overhead: "
                      << memorySizeString(s_Stats.TotalActualMemory -
                                          s_Stats.TotalReportedMemory)
                      << "\r\n";

            std::cout << "\r\n";

            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";
            std::cout << "|                                                    "
                         "        "
                         "P E A K S                                            "
                         "        "
                         "         |\r\n";
            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";

            std::cout << "              Allocation unit count: "
                      << insertCommas(s_Stats.PeakAllocUnitCount) << "\r\n";

            std::cout << "            Reported to application: "
                      << memorySizeString(s_Stats.PeakReportedMemory) << "\r\n";
            std::cout << "                             Actual: "
                      << memorySizeString(s_Stats.PeakActualMemory) << "\r\n";

            std::cout << "           Memory tracking overhead: "
                      << memorySizeString(s_Stats.PeakActualMemory -
                                          s_Stats.PeakReportedMemory)
                      << "\r\n";
            std::cout << "\r\n";

            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";
            std::cout << "|                                                    "
                         "  A C C "
                         "U M U L A T E D                                      "
                         "        "
                         "         |\r\n";
            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";

            std::cout << "              Allocation unit count: "
                      << memorySizeString(s_Stats.AccumulatedAllocUnitCount)
                      << "\r\n";
            std::cout << "            Reported to application: "
                      << memorySizeString(s_Stats.AccumulatedReportedMemory)
                      << "\r\n";
            std::cout << "                             Actual: "
                      << memorySizeString(s_Stats.AccumulatedActualMemory)
                      << "\r\n";
            std::cout << "\r\n";

            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";
            std::cout << "|                                                    "
                         "       "
                         "U N U S E D                                          "
                         "        "
                         "          |\r\n";
            std::cout << " ----------------------------------------------------"
                         "--------"
                         "-----------------------------------------------------"
                         "--------"
                         "--------- \r\n";

            std::cout << "    Memory allocated but not in use: "
                      << memorySizeString(CalcAllUnused()) << "\r\n";
            std::cout << "\r\n";

            std::cout
                << "Alloc.   Addr       Size       Addr       Size        "
                   "                BreakOn BreakOn              \r\n";
            std::cout << "Number Reported   Reported    Actual     Actual     "
                         "Unused    Method  Dealloc Realloc Allocated by \r\n";
            std::cout
                << "------ ---------- ---------- ---------- ---------- "
                   "---------- -------- ------- ------- "
                   "--------------------------------------------------- \r\n";

            for (const auto& [reportedAddress, allocUnit] : s_HashTable)
            {
                std::cout
                    << std::setw(6) << std::setfill('0')
                    << allocUnit->AllocationNumber << " "
                    << "0x" << std::setw(8) << std::setfill('0') << std::hex
                    << reinterpret_cast<uintptr_t>(
                           allocUnit->ReportedAddress)
                    << " "
                    << "0x" << std::setw(8) << std::setfill('0') << std::hex
                    << allocUnit->ReportedSize << " "
                    << "0x" << std::setw(8) << std::setfill('0') << std::hex
                    << reinterpret_cast<uintptr_t>(allocUnit->ActualAddress)
                    << " "
                    << "0x" << std::setw(8) << std::setfill('0') << std::hex
                    << allocUnit->ActualSize << " "
                    << "0x" << std::setw(8) << std::setfill('0') << std::hex
                    << CalcUnused(allocUnit) << " " << std::left << std::setw(8)
                    << kAllocationTypeNames[static_cast<size_t>(
                           allocUnit->AllocationType)]
                    << std::right << " "
                    << (allocUnit->BreakOnDealloc ? 'Y' : 'N') << "   "
                    << (allocUnit->BreakOnRealloc ? 'Y' : 'N') << "    "
                    << ownerString(allocUnit->SourceFile, allocUnit->SourceLine,
                                   allocUnit->SourceFunc)
                    << "\r\n"
                    << std::dec; // 恢复为十进制
            }
        }
    } // namespace Memory

} // namespace Moyu
