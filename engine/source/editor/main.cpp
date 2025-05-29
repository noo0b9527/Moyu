#include <stdlib.h>

#include <iostream>

#include <windows.h>

#include "host/moyu_host.h"
#include "main/main.h"

#include "editor/editor.h"

namespace moyu
{
    static EditorApp* app;

    static bool Init()
    {
        app = new EditorApp();
        return app->Init();
    }

    static void Exit() { app->Exit(); }

    static bool Load()
    {
        app->Load();
        return false;
    }

    static void Unload() { app->Unload(); }

    static void Update(float deltaTime) { app->Update(deltaTime); }

    static void Draw() { app->Draw(); }

} // namespace moyu

#if !defined(_CONSOLE)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine,
                   int nCmdShow)
{
#else
int main(int argc, const char** argv)
{
    // HINSTANCE instance = GetModuleHandle(nullptr);
#endif
    std::printf("Hello Editor Host Main\n");

    HMODULE moyu = LoadLibraryExA("moyu_shared.dll", nullptr, 0);
    if (moyu == nullptr)
    {
        std::cout << "Could not load fmapi.dll, Error :" << GetLastError()
                  << std::endl;
        return EXIT_FAILURE;
    }

    MOYU_HostAppDesc host_hook_desc = {
        .Init          = &moyu::Init,
        .Exit          = &moyu::Exit,
        .LoadContent   = &moyu::Load,
        .UnloadContent = &moyu::Unload,
        .Update        = &moyu::Update,
        .Draw          = &moyu::Draw,
    };

    MoyuMain moyu_main = (MoyuMain)(GetProcAddress(moyu, "MoyuMain"));
    int      rc        = moyu_main(argc, argv, &host_hook_desc);

    return rc;
}
