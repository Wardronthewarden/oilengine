#include "pch/oilpch.h"
#include "Application.h"

#include "oil/events/ApplicationEvent.h"
#include "oil/Log.h"

namespace oil{
oil::Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
}

Application::~Application()
{
}
void Application::Run()
{

    while (running){
        m_Window->OnUpdate();
    };
}
}