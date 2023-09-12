#pragma once

#include "core.h"


#include "Window.h"
#include "events/Event.h"
#include "oil/events/ApplicationEvent.h"
#include "oil/LayerStack.h"

namespace oil {
    class OIL_API Application{
    public:
        Application();
        virtual ~Application();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* layer);

        inline static Application& Get() { return *s_Instance; }
        inline Window& GetWindow() { return *m_Window; }
    private:
        bool OnWindowClose(WindowCloseEvent& e);

        std::unique_ptr<Window> m_Window;
        bool m_Running = true;
        LayerStack m_LayerStack;

    private:
        static Application* s_Instance;
    };

    //to be defined in a client app
    Application* CreateApplication();
}