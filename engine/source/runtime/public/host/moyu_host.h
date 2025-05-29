#pragma once

#include <stdbool.h>

typedef struct MOYU_HostAppDesc
{
    typedef bool (*AppInit)(void);
    typedef void (*AppExit)(void);

    typedef bool (*AppLoadContent)(void);
    typedef void (*AppUnloadContent)(void);

    typedef void (*AppUpdate)(float deltaTime);
    typedef void (*AppDraw)(void);

    AppInit  Init;
    AppExit Exit;

    AppLoadContent    LoadContent;
    AppUnloadContent UnloadContent;

    AppUpdate Update;
    AppDraw   Draw;
} MOYU_HostAppDesc;

typedef struct MOYU_HostPluginDesc
{
    // TODO: Plugin Sysytem???
} MOYU_HostPluginDesc;

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef __cplusplus
}
#endif /* __cplusplus */
