#pragma once

#include "oil/Layer.h"

#include "oil/events/ApplicationEvent.h"
#include "oil/events/KeyEvent.h"
#include "oil/events/MouseEvent.h"

namespace oil{
    class OIL_API ImGuiLayer : public Layer{
    public:
        ImGuiLayer();
        ~ImGuiLayer();


        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void OnImGuiRender() override;

        void Begin();
        void End();
    private:
        float m_Time = 0.0f;

    };
}