// MIT

using System;
using System.Runtime.InteropServices;

namespace MoyuEngine.Interop
{
    internal static partial class Runtime
    {
        [LibraryImport("Runtime.dll")]
        [UnmanagedCallConv(CallConvs = new Type[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
        public static partial void RuntimeMain(int argc, IntPtr argv);
    }
}
