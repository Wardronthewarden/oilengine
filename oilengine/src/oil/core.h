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

#define BIT(x) (1<<x)

#ifdef OIL_ENABLE_ASSERTS
    #define OIL_ASSERT(x, ...) {if(!(x)) {OIL_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
    #define OIL_CORE_ASSERT(x, ...) {if(!(x)) {OIL_CORE_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
#else
    #define OIL_ASSERT(x, ...)
    #define OIL_CORE_ASSERT(x, ...)
#endif