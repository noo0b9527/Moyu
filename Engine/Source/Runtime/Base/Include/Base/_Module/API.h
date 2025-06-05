#pragma once

#if defined(_WIN32) && defined(MOYU_DLL)
#if defined(MOYU_BASE)
#define MOYU_BASE_API __declspec(dllexport)
#define MOYU_BASE_CALL __cdecl
#else
#define MOYU_BASE_API __declspec(dllimport)
#define MOYU_BASE_CALL __cdecl
#endif
#else
#define MOYU_BASE_API
#define MOYU_BASE_CALL
#endif
