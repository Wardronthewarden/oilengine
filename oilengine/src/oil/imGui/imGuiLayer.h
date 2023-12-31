#pragma once

#include "oil/core/Layer.h"

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

        virtual void OnEvent(Event& e) override;

        void Begin();
        void End();

        void SetBlockEvents(bool block) { m_BlockEvents = block;}
        void SetDarkThemeColors();
    private:
        float m_Time = 0.0f;
        bool m_BlockEvents = true;

    };
}