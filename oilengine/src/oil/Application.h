#pragma once

#include "core.h"

namespace oil {
    class OIL_API Application{
    public:
        Application();
        virtual ~Application();

        void Run();
    };

    //to be defined in a client app
    Application* CreateApplication();
}