using System;
using System.Runtime.InteropServices;

namespace MoyuEngine
{
    public class Program
    {
        //[LibraryImport("user32.dll", StringMarshalling = StringMarshalling.Utf16, SetLastError = true)]
        //private static partial int MessageBoxW(IntPtr hWnd, string lpText, string lpCaption, uint uType);

        //[LibraryImport("Moyu.Servers.dll")]
        //private static partial int main();

        public static void Main(string[] args)
        {
            Console.WriteLine("Hello World");
        }
    }
}
