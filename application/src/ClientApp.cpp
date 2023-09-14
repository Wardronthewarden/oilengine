#include <oil.h>

#include "oilengine_export.h"


class ExampleLayer : public oil::Layer{
public:
    ExampleLayer()
        :Layer("Example"){

        }

    void OnUpdate() override{
        if (oil::Input::IsKeyPressed(OIL_KEY_TAB))
            OIL_TRACE("Tab key is pressed!");
    }

    void OnEvent(oil::Event& event) override{
       // OIL_TRACE("{0}", event);
    }
};

class Barrel : public oil::Application{
public:
    Barrel() {
        PushLayer(new ExampleLayer());
    };
    ~Barrel() {};



};


oil::Application* oil::CreateApplication(){
    OIL_INFO("Initialized client app!");
    return new Barrel();
}