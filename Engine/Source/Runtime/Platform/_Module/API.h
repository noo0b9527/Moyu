#pragma once

#if defined(_WIN32) && defined(MOYU_DLL)
#if defined(MOYU_PLATFORM)
#define MOYU_PLATFORM_API __declspec(dllexport)
#define MOYU_PLATFORM_CALL __cdecl
#else
#define MOYU_PLATFORM_API __declspec(dllimport)
#define MOYU_PLATFORM_CALL __cdecl
#endif
#else
#define MOYU_PLATFORM_API
#define MOYU_PLATFORM_CALL
#endif
