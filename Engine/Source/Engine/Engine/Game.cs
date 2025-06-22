// MIT

namespace Engine
{
    public abstract class Game
    {
        /// <summary>
        /// Called when the game is initialized.
        /// </summary>
        public abstract void OnInit();

        /// <summary>
        /// Called when the game is loaded resource.
        /// </summary>
        public abstract void OnLoad();

        /// <summary>
        /// Called when the game is updated.
        /// </summary>
        public abstract void OnUpdate(float deltaTime);

        /// <summary>
        /// Called when the game is rendered.
        /// </summary>
        public abstract void OnDraw();

        /// <summary>
        /// Called when the game is unloaded resource.
        /// </summary>
        public abstract void OnUnload();

        /// <summary>
        /// Called when the game is closed.
        /// </summary>
        public abstract void OnExit();
    }
}
