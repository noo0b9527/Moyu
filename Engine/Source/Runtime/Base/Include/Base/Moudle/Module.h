#pragma once

#include "Runtime/Base/Include/Base/_Module/API.h"

namespace Moyu
{
    class MOYU_BASE_API Module
    {
    public:
        Module()  = default;
        ~Module() = default;

        void Print();

    private:
    };

} // namespace Moyu
