#include <chrono>
#include <iostream>
#include <vector>

#include "Runtime/Base/Memory/Memory.h"

struct Dummy
{
    int   a;
    float b;
    char  buffer[32];
};

void TestCustomAllocator()
{
    constexpr size_t    count = 1'000'000;
    std::vector<Dummy*> pointers;
    pointers.reserve(count);

    auto start = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < count; ++i)
    {
        Dummy* ptr = Moyu::Malloc<Dummy>();
        pointers.push_back(ptr);
    }

    for (size_t i = 0; i < count; ++i)
    {
        Moyu::Free(pointers[i]);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed =
        std::chrono::duration<double, std::milli>(end - start).count();

   // Moyu::Memory::DumpMemoryReport();

    Moyu::ReportLeaks();

    std::cout << "[Moyu::Malloc/Free] Allocated and freed " << count
              << " objects in " << elapsed << " ms\n";
}

//void TestNativeNewDelete()
//{
//    constexpr size_t    count = 1'000'000;
//    std::vector<Dummy*> pointers;
//    pointers.reserve(count);
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//    for (size_t i = 0; i < count; ++i)
//    {
//        Dummy* ptr = static_cast<Dummy*>(malloc(sizeof(Dummy)));
//        pointers.push_back(ptr);
//    }
//
//    for (size_t i = 0; i < count; ++i)
//    {
//        free(pointers[i]);
//    }
//
//    auto end = std::chrono::high_resolution_clock::now();
//    auto elapsed =
//        std::chrono::duration<double, std::milli>(end - start).count();
//
//    std::cout << "[new/delete] Allocated and freed " << count << " objects in "
//              << elapsed << " ms\n";
//}

int main()
{
    std::cout << "Running allocator performance tests...\n";

    TestCustomAllocator();
    //TestNativeNewDelete();

    return 0;
}
