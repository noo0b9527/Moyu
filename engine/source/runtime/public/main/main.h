#pragma once

#include "host/moyu_host.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
    typedef int (*MoyuMain)(int argc, const char** argv,
                            MOYU_HostAppDesc* host_desc);
#ifdef __cplusplus
}
#endif /* __cplusplus */
