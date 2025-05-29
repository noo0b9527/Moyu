#include <iostream>

#include "host/moyu_host.h"

#include "platform/moyu_platform_defines.h"

#if defined(MOYU_PLATFORM_WINDOWS)
extern "C"
{
    __declspec(dllexport) int __cdecl MoyuMain(int argc, const char** argv,
                                               MOYU_HostAppDesc* host_app);
}
#endif

#if defined(MOYU_PLATFORM_WINDOWS)
__declspec(dllexport) int __cdecl MoyuMain(int argc, const char** argv,
                                           MOYU_HostAppDesc* host_app)
#endif
{
    if (host_app == nullptr)
    {
        return -1;
    }

    if (argc > 0)
    {
        std::printf("%s\n", argv[0]);
    }

    std::printf("Hello Moyu Dll Main\n");

    host_app->Init();
    host_app->LoadContent();

    host_app->Update(1.0f);
    host_app->Draw();

    host_app->UnloadContent();
    host_app->Exit();


    return 0;
}
