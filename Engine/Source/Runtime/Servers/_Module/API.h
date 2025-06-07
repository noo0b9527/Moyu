#pragma once

#if defined(_WIN32) && defined(MOYU_DLL)
#if defined(MOYU_SERVERS)
#define MOYU_SERVERS_API __declspec(dllexport)
#define MOYU_SERVERS_CALL __cdecl
#else
#define MOYU_SERVERS_API __declspec(dllimport)
#define MOYU_SERVERS_CALL __cdecl
#endif
#else
#define MOYU_SERVERS_API
#define MOYU_SERVERS_CALL
#endif
