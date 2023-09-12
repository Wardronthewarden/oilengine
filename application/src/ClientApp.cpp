#include <oil.h>

#include "oilengine_export.h"

class ExampleLayer : public oil::Layer{
public:
    ExampleLayer()
        :Layer("Example"){}

    void OnUpdate() override{
        OIL_INFO("ExampleLayer: Update");
    }

    void OnEvent(oil::Event& event) override{
        OIL_TRACE("{0}", event);
    }
};

class Barrel : public oil::Application{
public:
    Barrel() {
        PushLayer(new ExampleLayer());
        PushOverlay(new oil::ImGuiLayer());
    };
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    OIL_INFO("Initialized client app!");
    return new Barrel();
}