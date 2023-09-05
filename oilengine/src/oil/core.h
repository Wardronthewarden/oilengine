#pragma once

#ifdef OIL_PLATFORM_WINDOWS
    #include "oilengine_export.h"
#else
    #error Only windows supported currently!
#endif

#define BIT(x) (1<<x)