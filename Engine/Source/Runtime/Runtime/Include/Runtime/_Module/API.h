#pragma once

#include "Base/Config.h"


#if defined(MOYU_PLATFORM_WINDOWS) && defined(MOYU_DLL)
#if defined(RUNTIME)
#define MOYUAPI __declspec(dllexport)
#define MOYUCALL __cdecl
#else
#define MOYUAPI __declspec(dllimport)
#define MOYUCALL
#endif
#else
#define MOYUAPI
#define MOYUCALL
#endif



