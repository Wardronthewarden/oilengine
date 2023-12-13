#pragma once

#include <vector>
#include <map>
#include "Timer.h"

//TODO: create a better profiler for displaying scene stats!!!

namespace oil{
    struct ProfilerData{
        float RealDisplay;
        float SlowDisplay;
        float AverageDisplay;
    };

    class Profiler{
    public:
        Profiler();
        ~Profiler();

        ProfilerData* GetData() const;
    private:
        std::vector<ProfileResult> m_ProfileResult;
        std::map<std::string, ProfilerData> m_ProfilerData;
    };
}