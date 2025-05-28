#include <stdlib.h>

#include <iostream>

#include <windows.h>

typedef int (*DLL_MAIN)(int argc, const char** argv);

#if !defined(_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow)
{
#else
int main(int argc, const char** argv)
{
    // HINSTANCE instance = GetModuleHandle(nullptr);
#endif
    std::printf("Hello Exe Main\n");

    HMODULE moyu = LoadLibraryExA("moyu.dll", nullptr, 0);
    if (moyu == nullptr)
    {
        std::cout << "Could not load fmapi.dll, Error :" << GetLastError()
                  << std::endl;
        return EXIT_FAILURE;
    }

    DLL_MAIN moyu_main = (DLL_MAIN)(GetProcAddress(moyu, "MoyuMain"));
    int rc = moyu_main(argc, argv);

    return rc;
}
