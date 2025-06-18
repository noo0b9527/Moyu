#pragma once

#include "Base/Config.h"
#include "Base/Log.h"

#if !defined(MOYU_RELEASE)
#define MOYU_DEVELOPMENT_TOOLS 1
#endif

#if defined(MOYU_PLATFORM_WINDOWS) && defined(MOYU_DEVELOPMENT_TOOLS)
#define MOYU_DEBUG_BREAK() __debugbreak()
#endif

#if defined(MOYU_DEVELOPMENT_TOOLS)
#define MOYU_ASSERT(cond) \
    do \
    { \
        if (!(cond)) \
        { \
            Moyu::Logger logger = Moyu::Log(); \
            logger.Fatal(#cond); \
            MOYU_DEBUG_BREAK(); \
        } \
    } while (0)

#else
#define MOYU_ASSERT(cond)
#endif
