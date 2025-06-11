#include <iostream>

#include "Utilities/Log.h"
#include "Utilities/Assert.h"

int main()
{
    MOYU_ASSERT(false == false);
    Moyu::Log().Info("这是一个测试信息{}", 12121212);
    std::cout << "Hello";
    return 0;
}
