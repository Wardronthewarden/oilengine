#pragma once

#include <memory>

#include "core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace oil{

    class OIL_API Log{
    public:
        static void Init();

        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() {return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() {return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;


    };
}

#define OIL_CORE_FATAL(...) oil::Log::GetCoreLogger()->fatal(__VA_ARGS__)
#define OIL_CORE_ERROR(...) oil::Log::GetCoreLogger()->error(__VA_ARGS__)
#define OIL_CORE_WARN(...) oil::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define OIL_CORE_INFO(...) oil::Log::GetCoreLogger()->info(__VA_ARGS__)
#define OIL_CORE_TRACE(...) oil::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define OIL_FATAL(...) oil::Log::GetClientLogger()->fatal(__VA_ARGS__)
#define OIL_ERROR(...) oil::Log::GetClientLogger()->error(__VA_ARGS__)
#define OIL_WARN(...) oil::Log::GetClientLogger()->warn(__VA_ARGS__)
#define OIL_INFO(...) oil::Log::GetClientLogger()->info(__VA_ARGS__)
#define OIL_TRACE(...) oil::Log::GetClientLogger()->trace(__VA_ARGS__)