#pragma once

#include "core.h"
#include "events/Event.h"
#include "Window.h"


namespace oil {
    class OIL_API Application{
    public:
        Application();
        virtual ~Application();

        void Run();
    private:
        std::unique_ptr<Window> m_Window;
        bool running = true;
    };

    //to be defined in a client app
    Application* CreateApplication();
}