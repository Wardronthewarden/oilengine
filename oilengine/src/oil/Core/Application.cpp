#include "pch/oilpch.h"
#include "Application.h"

#include "oil/core/Log.h"

#include "oil/Renderer/Renderer.h"
#include "oil/storage/AssetManager.h"

#include "Input.h"

#include "glfw/glfw3.h"


namespace oil{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application* Application::s_Instance = nullptr;


oil::Application::Application(const std::string& name)
{
    OIL_CORE_ASSERT(!s_Instance, "Application already is initialized");
    s_Instance = this;
    m_Window = std::unique_ptr<Window>(Window::Create(WindowProps(name)));
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    AssetManager::Init("Assets", "Internal");
    Renderer::Init();

    m_ImGuiLayer = new ImGuiLayer;
    PushOverlay(m_ImGuiLayer);

    
}

Application::~Application()
{
}
void Application::Run()
{

    while (m_Running){
        float time = (float)glfwGetTime();
        Timestep timestep = time -m_LastFrameTime;
        m_LastFrameTime = time;

        if (!m_Minimized){
        for (Layer* layer : m_LayerStack)
            layer->OnUpdate(timestep);
        }

        m_ImGuiLayer->Begin();
        for (Layer* layer : m_LayerStack)
            layer->OnImGuiRender();
        m_ImGuiLayer->End();

        m_Window->OnUpdate();
    };
}
void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();){
        if (e.GetHandled())
            break;
        (*--it)->OnEvent(e);
    }
}
void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}
void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}
void Application::CloseApplication()
{
    m_Running = false;
}
bool Application::OnWindowClose(WindowCloseEvent &e)
{
    m_Running = false;
    return true;
}
bool Application::OnWindowResize(WindowResizeEvent &e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0){
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    

    return false;
}
}
