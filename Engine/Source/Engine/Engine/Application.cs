// MIT

namespace Engine
{
    public static class Application
    {
        public static void Run(Game game)
        {
            game.OnInit();
            game.OnLoad();
            game.OnUpdate(100f);
            game.OnDraw();
            game.OnUnload();
            game.OnExit();
        }
    }
}
