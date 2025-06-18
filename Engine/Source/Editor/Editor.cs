
using System;
using MoyuEngine;

namespace MoyuEditor
{
    internal class Editor : Application
    {
        static void Main(string[] args)
        {
            Editor editor = new Editor();
            editor.Run();
        }

        public override void Exit()
        {
            Console.WriteLine("Hello, Exit!");
        }

        public override bool Init()
        {
            Console.WriteLine("Hello, Init!");
            return true;
        }

        public override void Update(float deltaTime)
        {
            Console.WriteLine($"Hello, Update {deltaTime}!");
        }
    }
}
