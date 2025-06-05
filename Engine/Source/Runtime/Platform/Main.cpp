#include <iostream>

#include "Runtime/Base/Include/Base/Type/String.h"
#include "Runtime/Base/Include/Base/Moudle/Module.h"



int main()
{
    char buffer[100];
    Moyu::Module testModule = {};

    testModule.Print();

    auto size = Moyu::Printf(buffer, 100, "Hello {},Hello {}", 10, 10);
    std::cout << "Hello World" << std::endl;
    std::cout << buffer << " Size: " << size << std::endl;

    return 0;
}
