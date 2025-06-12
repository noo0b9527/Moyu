#include <iostream>

#include "Base/Log.h"
#include "Base/Assert.h"

int main()
{
    MOYU_ASSERT(false == false);
    Moyu::Log().Info("这是一个测试信息{}", 12121212);
    std::cout << "Hello";
    return 0;
}
