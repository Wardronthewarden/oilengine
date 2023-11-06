#pragma once

#include <memory>


#ifdef _WIN32
    // Windows x64/x86
    #ifdef _WIN64
        #define OIL_PLATFORM_WINDOWS
        #include "oilengine_export.h"
    #else
        #error "x86 Builds are currently not supported!"
    #endif

#elif defined(_APPLE_) || defined(_MACH_)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "IOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define OIL_PLATFORM_IOS
        #error "IOS currently not supported!"
    #elif TARGET_OS_MAC == 1
        #define OIL_PLATFORM_MACOS
        #error "MacOS is not supported!"
    #else
        #define OIL_API __declspec(dllimport)
        #error "Unknown Apple platform!"
    #endif

#elif defined(_ANDROID_)
    #define OIL_PLATFORM_ANDROID
    #error "Android currently not supported!"
#elif defined(_linux_)
    #define OIL_PLATFORM_LINUX
    #error "Linux currently not supported!"
#else
    #error "Unknown platform!"
#endif


#ifdef OIL_PLATFORM_WINDOWS
    #if OIL_DYNAMIC_LINK
        #ifdef OIL_BUILD_DLL
            #define OIL_API __declspec(dllexport)
        #else
            #define OIL_API __declspec(dllimport)
        #endif

    #else
        #define OIL_API
    #endif
#else
    #error Only windows supported currently!
#endif

#ifdef OIL_DEBUG
    #define OIL_ENABLE_ASSERTS
#endif

#ifdef OIL_ENABLE_ASSERTS
    #define OIL_ASSERT(x, ...) {if(!(x)) {OIL_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
    #define OIL_CORE_ASSERT(x, ...) {if(!(x)) {OIL_CORE_ERROR("Assertion falied: {0}", __VA_ARGS__); __debugbreak();}}
#else
    #define OIL_ASSERT(x, ...)
    #define OIL_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1<<x)

#define OIL_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace oil{

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}