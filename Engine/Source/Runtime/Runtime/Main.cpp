#include <iostream>

#include "Base/Log.h"
#include "Base/Assert.h"
#include "Base/Config.h"

#if defined(MOYU_PLATFORM_WINDOWS) && defined(MOYU_DLL)
extern "C" __declspec(dllexport) int RuntimeMain(int argc, char** argv);
#endif


int RuntimeMain(int argc, char** argv)
{
    if (argc)
    {

    }

    if (argv)
    {

    }

    Moyu::Log().Info("这是一个测试信息{}", 12121212);
    return 0;
}
