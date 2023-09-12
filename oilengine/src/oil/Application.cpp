#include "pch/oilpch.h"
#include "Application.h"

#include "oil/Log.h"

namespace oil{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
oil::Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

Application::~Application()
{
}
void Application::Run()
{

    while (m_Running){

        m_Window->OnUpdate();

        for (Layer* layer : m_LayerStack)
            layer->OnUpdate();
    };
}
void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();){
        (*--it)->OnEvent(e);
        if (e.GetHandled())
            break;
    }
}
void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
}
void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
}
bool Application::OnWindowClose(WindowCloseEvent &e)
{
    m_Running = false;
    return true;
}
}
