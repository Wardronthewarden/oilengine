#pragma once

#include "oil/core/Window.h"
#include "oil/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>


namespace oil{
    class WindowsWindow : public Window{
    public:
        WindowsWindow(const WindowProps& props);
        virtual ~WindowsWindow();

        void OnUpdate() override;

        inline unsigned int GetWidth() const override { return m_Data.Width; }
        inline unsigned int GetHeight() const override { return m_Data.Height; }

        // Win Attributes
        inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;

        inline virtual void* GetNativeWindow() const { return m_Window; }

        virtual void SetCursorMode(CursorMode mode) override;
        virtual CursorMode GetCursorMode() const override;
        virtual void SetCursorPosition(double x, double y) override;

    private:
        virtual void Init(const WindowProps& props);
        virtual void Shutdown();
    private:
        GLFWwindow* m_Window;
        GraphicsContext* m_Context;

        struct WindowData{
            std::string Title;
            unsigned int Width, Height;
            bool VSync;

            CursorMode cursorMode = CursorMode::CursorNormal;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };
}