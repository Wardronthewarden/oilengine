#pragma once

#include <memory>

#ifdef OIL_PLATFORM_WINDOWS
    #include "oilengine_export.h"
#else
    #error Only windows supported currently!
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