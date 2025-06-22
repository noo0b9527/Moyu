// MIT
using Engine;
using System;
using System.Runtime.InteropServices;

namespace Editor
{
    public class Editor : Game
    {

        [UnmanagedCallersOnly]
        private static void Entrance()
        {
            Editor editor = new Editor();
            Application.Run(editor);
        }

        public override void OnInit()
        {
            Console.WriteLine("OnInit");
        }

        public override void OnLoad()
        {
            Console.WriteLine("OnLoad");
        }

        public override void OnUpdate(float deltaTime)
        {
            Console.WriteLine("OnUpdate");
        }

        public override void OnDraw()
        {
            Console.WriteLine("OnDraw");
        }

        public override void OnUnload()
        {
            Console.WriteLine("OnUnload");
        }

        public override void OnExit()
        {
            Console.WriteLine("OnExit");
        }
    }
}
