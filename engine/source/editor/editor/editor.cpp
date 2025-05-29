#include "editor.h"

#include <iostream>

namespace moyu
{
    
    bool EditorApp::Init()
    {
        std::printf("Hello Editor Init\n");

        return true;
    }

    void EditorApp::Exit() { std::printf("Hello Editor Exit\n"); }

    bool EditorApp::Load()
    {
        std::printf("Hello Editor Load\n");

        return false;
    }

    void EditorApp::Unload() { std::printf("Hello Editor Unload\n"); }

    void EditorApp::Update(float deltaTime)
    {
        std::cout << "Hello Editor Update DeltaTime:" << deltaTime << std::endl;
    }

    void EditorApp::Draw() { std::printf("Hello Editor Draw\n"); }

} // namespace moyu
