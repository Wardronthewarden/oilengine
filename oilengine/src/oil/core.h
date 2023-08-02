#pragma once

#ifdef OIL_PLATFORM_WINDOWS
    #ifdef OIL_BUILD_DLL
        #define OIL_API __declspec(dllexport)
    #else 
        #define OIL_API __declspec(dllimport)
     #endif
#else
    #error Only windows suppoted currently
#endif