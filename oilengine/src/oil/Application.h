#pragma once

#include "core.h"
#include "events/Event.h"
#include "oil/events/ApplicationEvent.h"
#include "Window.h"


namespace oil {
    class OIL_API Application{
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);
    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
    };

    //to be defined in a client app
    Application* CreateApplication();
}