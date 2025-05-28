#include <iostream>

#include "platform/moyu_platform_defines.h"



#if defined(MOYU_PLATFORM_WINDOWS)
extern "C"
{
    __declspec(dllexport) int __cdecl MoyuMain(int argc, const char** argv);
}
#endif

#if defined(MOYU_PLATFORM_WINDOWS)
__declspec(dllexport) int __cdecl MoyuMain(int argc, const char** argv)
#endif
{
    if (argc > 0)
    {
        std::printf("%s\n", argv[0]);
    }

    std::printf("Hello Moyu Main");

    return 0;
}
