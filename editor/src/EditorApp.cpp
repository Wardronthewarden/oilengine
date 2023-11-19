#include <oil.h>
#include <oil/core/EntryPoint.h>

#include "EditorLayer.h"

namespace oil{
    class OilEditor : public oil::Application{
    public:
        OilEditor()
            : Application("Oil Editor"){
            PushLayer(new EditorLayer());
        }

        ~OilEditor(){}
    };

    oil::Application* oil::CreateApplication(){
        return new OilEditor();
    }
}