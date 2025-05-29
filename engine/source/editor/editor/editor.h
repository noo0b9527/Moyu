#pragma once

namespace moyu
{

    class EditorApp
    {
    public:
        EditorApp()  = default;
        ~EditorApp() = default;

        EditorApp(const EditorApp&)      = delete;
        EditorApp& operator=(EditorApp&) = delete;

        EditorApp(EditorApp&&)            = delete;
        EditorApp& operator=(EditorApp&&) = delete;

        bool Init();
        void Exit();

        bool Load();
        void Unload();

        void Update(float deltaTime);
        void Draw();
    };
} // namespace moyu
