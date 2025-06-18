// MIT

using System.Runtime.InteropServices;
using System;
using MoyuEngine.Interop;

namespace MoyuEngine
{
    public abstract class Application
    {
        private static Application s_current;


        public abstract bool Init();
        public abstract void Update(float deltaTime);
        public abstract void Exit();


        public void Run()
        {
            s_current = this;

            s_current.Init();
            s_current.Update(10f);
            s_current.Exit();

            Runtime.RuntimeMain(0, IntPtr.Zero);
        }

    }
}
