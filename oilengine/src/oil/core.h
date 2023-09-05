#pragma once

#ifdef OIL_PLATFORM_WINDOWS
    #include "oilengine_export.h"
#else
    #error Only windows supported currently!
#endif

#define BIT(x) (1<<x)

#ifdef OIL_ENABLE_ASSERTS
    #define OIL_ASSERT(x, ...) {if(!(x)) {OIL_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
    #define OIL_CORE_ASSERT(x, ...) {if(!(x)) {OIL_CORE_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
#else
    #define OIL_ASSERT(x, ...)
    #define OIL_CORE_ASSERT(x, ...)
#endif