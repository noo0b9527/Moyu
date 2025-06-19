#include <iostream>


int main(int argc, char* argv[])
{
    if (argc)
    {
        std::cout << "Argument count: " << argc << std::endl;
    }
    if (argv)
    {
        for (int i = 0; i < argc; ++i)
        {
            std::cout << "Argument " << i << ": " << argv[i] << std::endl;
        }
    }
    std::cout << "这是一个测试信息" << 12121212 << std::endl;
    return 0;
}
